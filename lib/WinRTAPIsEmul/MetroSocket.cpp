#include "MetroSocket.h"

#ifdef _DEBUG
#define WRTSOCK_DBG_ERROR(...)			WRTSockDebugOut(1, __VA_ARGS__)
#define WRTSOCK_DBG_WARNING(...)		WRTSockDebugOut(2, __VA_ARGS__)
#define WRTSOCK_DBG_INFO(...)			WRTSockDebugOut(3, __VA_ARGS__)
#else
#define WRTSOCK_DBG_ERROR(...)
#define WRTSOCK_DBG_WARNING(...)
#define WRTSOCK_DBG_INFO(...)
#endif
#define WRTSOCK_BREAKIF_ERROR
#define USE_WORKITEM_HANDLER
#define MAX_TCP_SEGMENT_SIZE 65535
#define MAX_TCP_BUFFER_SIZE	(MAX_TCP_SEGMENT_SIZE*10)
#define MAX_UDP_BUFFER_SIZE	(1024*1024)
#define MAX_NUM_SOCKET 1024
//#define UPDATE_WSA_BY_THREAD

void __Debug_out(const char* pFunction, int lineNumber, const char* pFormat, ...)
{
#if defined _DEBUG && ! defined OS_W10
	char out[10240];
	memset(out, 0, 10240);
	va_list body;
	_crt_va_start(body, pFormat);
	vsprintf_s(out, pFormat, body);
	_crt_va_end(body);
	strcat_s(out, "\r\n");

	printf_s(out);
	OutputDebugStringA(out);
#endif
}

std::string BufferToString(unsigned char* buff, size_t buffsize)
{
	char chHex[102410];
	memset(chHex, 0, 10240);
	for (size_t i=0; i < buffsize; i++)
	{
		sprintf((chHex + i*3), "%02x ", (unsigned char)buff[i]);	
	}
	std::string ret(chHex, buffsize * 3);
	return ret;
}

void WRTSockDebugOut(int type, const char* format, ...)
{
	#define DBG_OUT_LEN	(1024*10)

	char outstr[DBG_OUT_LEN] = {0};
	memset(outstr, 0, DBG_OUT_LEN);
	if (type == 1)
		strcpy(outstr, "[WRTSOCK_ERROR]");
	else if (type == 2)
		strcpy(outstr, "[WRTSOCK_WARNING]");
	else
		strcpy(outstr, "[WRTSOCK_INFO]");
	int headLen = strlen(outstr);

	va_list argptr;
	va_start(argptr, format);
	vsprintf((outstr+headLen), format, argptr);
	va_end(argptr);
#ifdef _DEBUG
	OutputDebugStringA(outstr);
	OutputDebugStringA("\n");
#endif
}

const int	HOST_BUF_LEN = 10;
struct hostent	*l_virtual_host[HOST_BUF_LEN];
int l_vhost_count = 0;
ISocketBase** g_ListWRTSocket = NULL;
std::vector<ISocketBase*>		g_ListWRTSocketPending;
int wsa_startup_count = 0;
mutex	g_ResolveHostMtx;
#ifdef UPDATE_WSA_BY_THREAD
HANDLE	g_UpdateSocketPendingEvt = NULL;
thread	g_UpdateSocketPendingThr;
#endif
mutex	g_UpdateSockePendingMtx;


int GetNetAddressType(const char *str)
{
	String^ name = atops(str);
	HostName^ hName = ref new HostName(name);
	if (hName != nullptr)
	{
		if(hName->Type == HostNameType::Ipv4)
			return 1;
		else if(hName->Type == HostNameType::Ipv6)
			return 2;
	}
	return 0;
}
unsigned int inet_addr(const char *str)
{ 
    unsigned int a,b,c,d=0;
	unsigned int ret = 0;
    int res = sscanf_s(str,"%d.%d.%d.%d",&a,&b,&c,&d);
	if (res < 4)
		return -1;
	ret = (d << 24) | (c << 16) | (b << 8) | a;
	return ret;
}

char *inet_ntoa(in_addr in)
{
	__declspec( thread ) static char iAddr[16] = {0};
	memset(iAddr, 0, 16);
	sprintf_s(iAddr,"%d.%d.%d.%d", in.s_net, in.s_host, in.s_lh, in.s_impno);	
	return iAddr;
}
bool IsValidSock(int fd)
{
	return (fd >= 0 && fd < MAX_NUM_SOCKET && g_ListWRTSocket[fd] != NULL);
}
int FD_ISSET(int fd, struct fd_set FAR *set)
{
	if (!set || !IsValidSock(fd))
		return false;
	for (u_int i = 0; i < set->fd_count; i++)
	{
		if (fd == set->fd_array[i])
			return true;
	}
	return false;
}
static DWORD WINAPI WSAUpdatePendingTask() {
#ifdef UPDATE_WSA_BY_THREAD
	while(true)
	{
		if (wsa_startup_count == 0)
			break;
		WaitForSingleObjectEx(g_UpdateSocketPendingEvt, INFINITE, false);
		lock_guard<mutex> lock(g_UpdateSockePendingMtx);
		for(size_t i = 0; i < g_ListWRTSocketPending.size(); i++)
		{
			ISocketBase *it = g_ListWRTSocketPending.back();
			if (it->IsIdle())
			{
				delete it;
				g_ListWRTSocketPending.pop_back();
			}
		}
		ResetEvent(g_UpdateSocketPendingEvt);
	}
#else
	lock_guard<mutex> lock(g_UpdateSockePendingMtx);
	for(size_t i = 0; i < g_ListWRTSocketPending.size(); i++)
	{
		ISocketBase *it = g_ListWRTSocketPending.back();
		if (it->GetEvent() == IDLE)
		{
			delete it;
			g_ListWRTSocketPending.pop_back();
		}
	}
#endif
	return 0;
}
int WSAStartup(WORD  wVersionRequested, LPWSADATA  lpWSAData)
{	
	if (wsa_startup_count == 0)
	{
		IVectorView<HostName^>^ listhost = NetworkInformation::GetHostNames();
		g_ListWRTSocketPending.clear();
		if (listhost->Size <= 0)
		{
			return WSASYSNOTREADY;
		}
		for(u_int i = 0; i< listhost->Size; i++)
		{
			HostName^ hostname = listhost->GetAt(i);
			if (hostname != nullptr)
			{
				String^ host = listhost->GetAt(i)->DisplayName;
				SOCK_DEBUG_OUT("WRTSocket::WSAStartup:  Machine Address:%s\n", pstos(host).c_str());
			}
		}

		for (int i=0; i<HOST_BUF_LEN; i++ )
		{
			l_virtual_host[i] = NULL;
		}
		l_vhost_count = 0;

		g_ListWRTSocket = new ISocketBase*[MAX_NUM_SOCKET];
		for (int i = 0; i < MAX_NUM_SOCKET; i++)
		{
			g_ListWRTSocket[i] = NULL;
		}
	}
	wsa_startup_count++;
#ifdef UPDATE_WSA_BY_THREAD
	if (wsa_startup_count == 1)
	{
		g_UpdateSocketPendingEvt = CreateEventExW(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
		g_UpdateSocketPendingThr = std::thread(WSAUpdatePendingTask);
	}
#endif
	if (wVersionRequested >= MAKEWORD(1, 1))
	{
		lpWSAData->wVersion = wVersionRequested;
		lpWSAData->wHighVersion = MAKEWORD(2, 2);
		return 0;
	}
	return WSAVERNOTSUPPORTED;
}
int WSACleanup(void)
{
	if (wsa_startup_count > 0)
	{
		wsa_startup_count--;
		if (wsa_startup_count==0)
		{
			for ( int i=0; i<l_vhost_count; i++ )
			{
				SOCK_API_DEL( l_virtual_host[i]->h_name );		
				for (int j=0; j<1; j++ )
				{
					SOCK_API_DEL( l_virtual_host[i]->h_addr_list[j] );
					l_virtual_host[i]->h_addr_list[j] = NULL;
				}
				SOCK_API_DEL( l_virtual_host[i]->h_addr_list );
				SOCK_API_DEL( l_virtual_host[i] );
				l_virtual_host[i] = NULL;
			}
			l_vhost_count = 0;

			for (int i = 0; i < MAX_NUM_SOCKET; i++)
			{
				SOCK_API_DEL(g_ListWRTSocket[i]);
			}
#ifdef UPDATE_WSA_BY_THREAD
			SetEvent(g_UpdateSocketPendingEvt);
#endif
		}
	}
	return 0;
}
int WSAGetLastError(void)
{
	return GetLastError();
}
void WSASetLastError(int iError)
{
	SetLastError(iError);
}
#if ! defined OS_W8 && ! defined OS_WP8 && ! defined OS_W10
void Sleep(unsigned long dwMilliseconds)
{
	ThreadEmulation::Sleep(dwMilliseconds);
}
#endif
int setsockopt (int fd, int level, int optname, const char FAR * optval, int optlen){
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	if (level == SOL_SOCKET && optname == SO_GROUP_PRIORITY && optlen == sizeof(int))
	{
		int value = 0;
		memcpy((char*)&value, optval, optlen);
		g_ListWRTSocket[fd]->SetRecvBlockSize(value);
	}
	return 0;
}
int getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len){
	return 0;
}
int ioctlsocket (int fd, long cmd, u_long FAR *argp)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	if (cmd == FIONBIO)
	{
		//TODO: set non-blocking
		return 0;
	}
	else
	{
		return SOCKET_ERROR;
	}
}
struct protoent* getprotobyname(const char * name)
{
	static struct protoent protocol = {"tcp", NULL, IPPROTO_TCP};
	return &protocol;
}
u_short htons(u_short usnum){
	return (usnum >> 8) | (usnum << 8);
}
u_short ntohs(u_short usnum){
	return htons(usnum);
}
u_long htonl(u_long hostlong){
	return ((((uint32)(hostlong) & 0xff000000) >> 24) | (((uint32)(hostlong) & 0x00ff0000) >> 8) | (((uint32)(hostlong) & 0x0000ff00) << 8) | (((uint32)(hostlong) & 0x000000ff) << 24));
}
u_long ntohl(u_long netlong){
	return htonl(netlong);
}
int gethostname(char *name, int namelen)
{
	if (name)
	{
		memset(name, 0, namelen);
		IVectorView<HostName^>^ listhost = NetworkInformation::GetHostNames();
		for(int i = 0; i< listhost->Size; i++)
		{
			HostName^ hName = listhost->GetAt(i);
			if (hName != nullptr && hName->DisplayName != nullptr && hName->Type != HostNameType::Ipv6 && hName->Type != HostNameType::Ipv4)
			{
				std::string host_name = pstos(hName->DisplayName);
				if(host_name.size() > namelen)
				{
					SetLastError(WSAEFAULT);
					return 1;
				}
				strcpy(name, host_name.c_str());
				return 0;
			}
		}
	}
	else
	{
		SetLastError(WSAEFAULT);
	}
	return 1;
}
int getlocalip(char *ip, int namelen)
{
	if (ip)
	{
		memset(ip, 0, namelen);
		IVectorView<HostName^>^ listhost = NetworkInformation::GetHostNames();
		for(int i = 0; i< listhost->Size; i++)
		{
			HostName^ hName = listhost->GetAt(i);
			if (hName != nullptr && hName->DisplayName != nullptr && hName->Type == HostNameType::Ipv4)
			{
				std::string host_name = pstos(hName->DisplayName);
				if(host_name.size() > namelen)
				{
					SetLastError(WSAEFAULT);
					return -1;
				}
				strcpy(ip, host_name.c_str());
				return 0;
			}
		}
	}
	else
	{
		SetLastError(WSAEFAULT);
	}
	return -1;
}
std::vector<std::string> ResolveHostName(std::string hostname) 
{
	std::vector<std::string> hosts_ip;

	//Check is local name
	IVectorView<HostName^>^ listhost = NetworkInformation::GetHostNames();
	std::string local_ip = "";
	bool isLocalIP = false;
	if (hostname == "localhost" || hostname == "::1" || hostname == "::")
		isLocalIP = true;
	for(int i = 0; i< listhost->Size; i++)
	{
		HostName^ hName = listhost->GetAt(i);
		if (hName != nullptr && hName->DisplayName != nullptr && hName->Type != HostNameType::Ipv6 && hName->Type != HostNameType::Ipv4)
		{
			std::string host_name = pstos(hName->DisplayName);
			if(host_name == hostname)
			{
				isLocalIP = true;
			}
		} else if (hName != nullptr && hName->DisplayName != nullptr && hName->Type == HostNameType::Ipv4)
		{
			local_ip = pstos(hName->DisplayName);
		}
	}
	if (local_ip != "" && isLocalIP)
	{
		hosts_ip.push_back(local_ip);
		return hosts_ip;
	}

	HANDLE threadHandle = CreateEventExW(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
	HANDLE completionEvent;
	if (!threadHandle)
		return hosts_ip;	      
    if (!DuplicateHandle(GetCurrentProcess(), threadHandle, GetCurrentProcess(), &completionEvent, 0, false, DUPLICATE_SAME_ACCESS))
    {
        CloseHandle(threadHandle);
        return hosts_ip;
    }

	reader_writer_lock hosts_ip_mutex; 
	auto workItemHandler = ref new WorkItemHandler([=, &hosts_ip, &hosts_ip_mutex](IAsyncAction^) 
	{ 
		// Run the user callback. 
		HostName^ remote_name = ref new HostName(stops(hostname)); 
		try 
		{             
			IVectorView<EndpointPair^>^ listEndPointPair = task<IVectorView<EndpointPair^>^>(DatagramSocket::GetEndpointPairsAsync(remote_name, L"0")).get(); 
			if(listEndPointPair != nullptr) 
			{ 
				hosts_ip_mutex.lock(); 
				for(int i = 0; i < listEndPointPair->Size; i++) 
				{  
					EndpointPair^ endpoint = listEndPointPair->GetAt(0); 
					if (endpoint != nullptr && endpoint->RemoteHostName != nullptr &&  
					(endpoint->RemoteHostName->Type == HostNameType::Ipv4 || endpoint->RemoteHostName->Type == HostNameType::Ipv6) 
					) 
					{  
						std::wstring wip(endpoint->RemoteHostName->CanonicalName->Data());					
						std::string host_ip(wip.begin(), wip.end());
						std::vector<std::string>::iterator curIter = std::find(hosts_ip.begin(), hosts_ip.begin() + hosts_ip.size(), host_ip);
						if(curIter == hosts_ip.end())
							hosts_ip.push_back(host_ip);
					} 
				} 
				hosts_ip_mutex.unlock();
			} 
		} 
		catch (...) { } 
		SetEvent(completionEvent);
	}, CallbackContext::Any); 
	ThreadPool::RunAsync(workItemHandler, WorkItemPriority::Normal, WorkItemOptions::TimeSliced);

	WaitForSingleObjectEx(completionEvent, INFINITE, false);
	CloseHandle(threadHandle);
    CloseHandle(completionEvent);

	hosts_ip_mutex.lock();
	std::vector<std::string> hosts_res = hosts_ip; 
	hosts_ip_mutex.unlock();  
	return hosts_res;
}

struct hostent *gethostbyname(const char *name)
{
	lock_guard<mutex> lock(g_ResolveHostMtx);
	//this function only support IPV4
	for (int i=0; i<l_vhost_count; i++)
	{
		if (l_virtual_host[i] != NULL && (strcmp( name, l_virtual_host[i]->h_name ) == 0))
		{
			return l_virtual_host[i];
		}
	}


	if (l_vhost_count < HOST_BUF_LEN)
	{
		unsigned int ipAddr = 0;
		{
			char hostname[256] = {0};
			std::vector<std::string> list_ip = ResolveHostName(name);
			if (list_ip.size() > 0)
			{
				std::string ip = list_ip[0];
				memcpy(hostname, ip.c_str(), ip.size());
			}
			ipAddr = inet_addr(hostname);
			if (ipAddr == (-1))
			{
				return NULL;
			}
		}
		
		hostent *pHost = l_virtual_host[l_vhost_count];
		if (pHost)
		{
			SOCK_API_DEL (pHost->h_name);	
			SOCK_API_DEL_ARRAY(pHost->h_addr_list, 1);
			delete (pHost);
			l_virtual_host[l_vhost_count] = NULL;
			pHost = NULL;
		}

		pHost = new hostent();
		if (pHost)
		{
			l_virtual_host[l_vhost_count] = pHost;
			pHost->h_length = 4;
			pHost->h_addrtype = AF_INET;
			pHost->h_name = SOCK_API_STRNEW( name );
			pHost->h_addr_list = new char * [2];		

			int i = 0;
			for (; i<1; i++ )
			{
				//this is a bug, while the ip have 0 (eg. 10.15.0.64 ).we will get 10.15.0.253.
				//l_virtual_host[l_vhost_count]->h_addr_list[i] = 	XP_API_STRNEW( server->h_addr_list[i] );
			
				pHost->h_addr_list[i] = new char[ 4 ];
				memcpy(pHost->h_addr_list[i] , &ipAddr, 4 ) ;
			}
			pHost->h_addr_list[i] = NULL;
			l_vhost_count++;
			return pHost;
		}
		else
		{
			return NULL;
		}		
	}
	else
	{
		l_vhost_count -- ;
		return gethostbyname( name );
	}
	return NULL;
}

struct hostent *gethostbyaddr(const void *addr, int len, int type)
{
	if (type == AF_INET)
	{
		char formatedAddr[4] = {0};
		if (len == 4)
		{
			memcpy(formatedAddr, addr, 4);
			for (int i=0; i<l_vhost_count; i++)
			{
				if (strncmp( (char*)formatedAddr, l_virtual_host[i]->h_addr_list[0], 4) == 0)
				{
					return l_virtual_host[i];
				}
			}
			in_addr naddr;
			memcpy(&naddr, addr, sizeof(in_addr));
			char* dotIP = inet_ntoa(naddr);
			return gethostbyname(dotIP);
		}
	}
	return NULL;
}
int getpeername(int fd, struct sockaddr FAR* name, int FAR* namelen)
{
	if (!IsValidSock(fd))
	{
		name = NULL;
		*namelen = 0;
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	memset(name, 0, sizeof(sockaddr));
	u_short hostPort = g_ListWRTSocket[fd]->getremoteport();
	if (hostPort > 0)
		((sockaddr_in*)name)->sin_port = htons(hostPort);
	else
		((sockaddr_in*)name)->sin_port = 0;

	int error = 0;

	std::string dstHost = g_ListWRTSocket[fd]->getremoteaddr();
	if (dstHost != "")
	{
		if (GetNetAddressType(dstHost.c_str()) != 0)
		{
			unsigned int iAddr = inet_addr((char*)dstHost.c_str());
			if (iAddr != (-1))
				memcpy(&((sockaddr_in*)name)->sin_addr, &iAddr, 4);
			else
				error = SOCKET_ERROR;
		}
		else
		{
			//RemoteHostName is not in format xxx.xxx.xxx.xxx
			hostent* host = gethostbyname(dstHost.c_str());
			memcpy(&((sockaddr_in*)name)->sin_addr, host->h_addr_list[0], 4);			
		}
	}
	else
	{
		int unknow_addr = 0;
		memcpy(&((sockaddr_in*)name)->sin_addr, &unknow_addr, 4);
	}
	((sockaddr_in*)name)->sin_family = AF_INET;
	*namelen = sizeof(sockaddr_in);
	return error;
}
int getsockname(int fd, struct sockaddr FAR* name, int FAR* namelen)
{
	if (!IsValidSock(fd))
	{
		name = NULL;
		*namelen = 0;
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	memset(name, 0, sizeof(sockaddr));
	int iAddr = 0;
	u_short hostPort = g_ListWRTSocket[fd]->getlocalport();
	if (hostPort > 0)
		((sockaddr_in*)name)->sin_port = htons(hostPort);
	else
		((sockaddr_in*)name)->sin_port = 0;

	int error = 0;

	std::string localHost = g_ListWRTSocket[fd]->getlocaladdr();
	if (localHost != "")
	{
		if (GetNetAddressType((char*)localHost.c_str()) != 0)
		{
			unsigned int iAddr = inet_addr((char*)localHost.c_str());
			if (iAddr != (-1))
				memcpy(&((sockaddr_in*)name)->sin_addr, &iAddr, 4);
			else
				error = SOCKET_ERROR;
		}
		else
		{
			//LocalHostName is not in format xxx.xxx.xxx.xxx
			hostent* host = gethostbyname(localHost.c_str());
			memcpy(&((sockaddr_in*)name)->sin_addr, host->h_addr_list[0], 4);			
		}
	} else
	{
		int unkown_addr = 0;
		memcpy(&((sockaddr_in*)name)->sin_addr, &unkown_addr, 4);
	}

	((sockaddr_in*)name)->sin_family = AF_INET;
	*namelen = sizeof(sockaddr_in);
	return error;
}

int close(int fd)
{
	if (wsa_startup_count <= 0)
	{
		WSASetLastError(WSANOTINITIALISED);
		return SOCKET_ERROR; 
	}

	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	int iRet = g_ListWRTSocket[fd]->closesocket();
	lock_guard<mutex> lock(g_UpdateSockePendingMtx);
	g_ListWRTSocketPending.push_back(g_ListWRTSocket[fd]);
	g_ListWRTSocket[fd] = NULL;
	return iRet;

}
int closesocket(int fd)
{
	return close(fd);
}
int shutdown(int fd, int how)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	return close(fd);
}
int connect(int fd, const sockaddr* foreignAddress, unsigned int addressLength)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	if (!foreignAddress)
	{
		return g_ListWRTSocket[fd]->connect("", 0);
	}

	int iRet = 0;
	{
		char *strIp = inet_ntoa(((sockaddr_in*)foreignAddress)->sin_addr);
		u_short netPort = ((sockaddr_in*)foreignAddress)->sin_port;
		iRet = g_ListWRTSocket[fd]->connect(strIp, ntohs(netPort));
	}
	return iRet;
}

int sendto(int fd, const char FAR * buf, int len, int flags, const struct sockaddr FAR * foreignAddress, int tolen)
{
	if(!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	if (!foreignAddress)
	{
		return g_ListWRTSocket[fd]->sendto(buf, len, "", 0);
	}

	int iRet = 0;
	{
		char *strIp = inet_ntoa(((sockaddr_in*)foreignAddress)->sin_addr);
		u_short netPort = ((sockaddr_in*)foreignAddress)->sin_port;
		iRet = g_ListWRTSocket[fd]->sendto(buf, len, strIp, ntohs(netPort));
	}
	return iRet;
}

int WSASendTo(
    SOCKET fd,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent,
    DWORD dwFlags,
    const struct sockaddr FAR * lpTo,
    int iTolen,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    )
{
	int iRet = 0;
	(*lpNumberOfBytesSent) = 0;
	if(!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		iRet = SOCKET_ERROR;
	}

	if (!lpTo)
	{		
		int iRet = g_ListWRTSocket[fd]->wsasendto(lpBuffers, dwBufferCount, "", 0);
		if (iRet != SOCKET_ERROR)
			(*lpNumberOfBytesSent) = iRet;
	}
	else
	{
		char *strIp = inet_ntoa(((sockaddr_in*)lpTo)->sin_addr);
		u_short netPort = ((sockaddr_in*)lpTo)->sin_port;
		int iRet = g_ListWRTSocket[fd]->wsasendto(lpBuffers, dwBufferCount, strIp, ntohs(netPort));
		if (iRet != SOCKET_ERROR)
			(*lpNumberOfBytesSent) = iRet;
	}
	return iRet;
}

int send(int fd, const void* msg, unsigned int msgLength, int flags)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	return g_ListWRTSocket[fd]->send((const char*)msg, msgLength);
}

int recvfrom(int fd, char FAR* buf, int len, int flags, struct sockaddr FAR* foreignAddress, int FAR* fromlen)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	if (!foreignAddress)
	{
		return g_ListWRTSocket[fd]->recv((char*)buf, len, flags);
	}

	int hostPort = 0;
	std::string strRemoteHost;
	int iRet = g_ListWRTSocket[fd]->recvfrom(buf, len, flags, strRemoteHost, hostPort);;
	if (iRet <= 0)
		return iRet;
	const char* strIp = strRemoteHost.c_str();

	((sockaddr_in*)foreignAddress)->sin_port = htons(hostPort);	
	if (GetNetAddressType(strIp) != 0)
	{
		unsigned int iAddr = inet_addr(strIp);
		if (iAddr != (-1))
			memcpy(&((sockaddr_in*)foreignAddress)->sin_addr, &iAddr, 4);
	}
	else
	{
		hostent* host = gethostbyname(strIp);
		if (host)
			memcpy(&((sockaddr_in*)foreignAddress)->sin_addr, host->h_addr_list[0], 4);
		else
			memset(&((sockaddr_in*)foreignAddress)->sin_addr, 0, 4);
	}
	return iRet;
}

int WSARecvFrom(
    SOCKET fd,
    LPWSABUF lpBuffers,
    DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd,
    LPDWORD lpFlags,
    struct sockaddr FAR * lpFrom,
    LPINT lpFromlen,
    LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    )
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	if (!lpFrom)
	{
		//wsarecv not implemented.
		WRTSOCK_DBG_ERROR("Not Implemented");
		return SOCKET_ERROR;
	}

	(*lpNumberOfBytesRecvd) = 0;
	int hostPort = 0;
	std::string strRemoteHost;
	int iRet = g_ListWRTSocket[fd]->wsarecvfrom(lpBuffers, dwBufferCount, strRemoteHost, hostPort);
	if (iRet <= 0)
	{
		return iRet;
	}

	(*lpNumberOfBytesRecvd) = iRet;
	const char* strIp = strRemoteHost.c_str();

	((sockaddr_in*)lpFrom)->sin_port = htons(hostPort);	
	if (GetNetAddressType(strIp) != 0)
	{
		unsigned int iAddr = inet_addr(strIp);
		if (iAddr != (-1))
			memcpy(&((sockaddr_in*)lpFrom)->sin_addr, &iAddr, 4);
	}
	else
	{
		hostent* host = gethostbyname(strIp);
		if (host)
			memcpy(&((sockaddr_in*)lpFrom)->sin_addr, host->h_addr_list[0], 4);
		else
			memset(&((sockaddr_in*)lpFrom)->sin_addr, 0, 4);
	}
	return 0;
}

int recv(int fd, void* recvBuffer, unsigned int bufferLength, int flags)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	return g_ListWRTSocket[fd]->recv((char*)recvBuffer, bufferLength, flags);
}

int bind(int fd, const sockaddr* localAddress, unsigned int addressLength)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	if (!localAddress)
	{
		return g_ListWRTSocket[fd]->bind("0.0.0.0", 0);
	}

	int iRet = 0;
	{
		char *strIp = inet_ntoa(((sockaddr_in*)localAddress)->sin_addr);
		int hostPort = ntohs(((sockaddr_in*)localAddress)->sin_port);
		iRet = g_ListWRTSocket[fd]->bind(strIp, hostPort);
	}

	return iRet;
}
int listen(int fd, int queueLimit)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}
	return g_ListWRTSocket[fd]->listen(queueLimit);
}
int accept(int fd, sockaddr* clientAddress, int *addressLength)
{
	if (!IsValidSock(fd))
	{
		WSASetLastError(WSAENOTSOCK);
		return SOCKET_ERROR;
	}

	//TODO
	int iRet = g_ListWRTSocket[fd]->accept();
	if (IsValidSock(iRet) && clientAddress)
	{
		memset(clientAddress, 0, sizeof(sockaddr));
		u_short hostPort = g_ListWRTSocket[iRet]->getremoteport();
		if (hostPort > 0)
			((sockaddr_in*)clientAddress)->sin_port = htons(hostPort);
		else
			((sockaddr_in*)clientAddress)->sin_port = 0;

		std::string dstHost = g_ListWRTSocket[iRet]->getremoteaddr();
		if (dstHost != "")
		{			
			if (GetNetAddressType((char*)dstHost.c_str()) != 0)
			{
				unsigned int iAddr = inet_addr((char*)dstHost.c_str());
				if (iAddr != (-1))
					memcpy(&((sockaddr_in*)clientAddress)->sin_addr, &iAddr, 4);
			}
			else
			{
				//RemoteHostName is not in format xxx.xxx.xxx.xxx
				hostent* host = gethostbyname(dstHost.c_str());
				if (host)
					memcpy(&((sockaddr_in*)clientAddress)->sin_addr, host->h_addr_list[0], 4);
				else
					memcpy(&((sockaddr_in*)clientAddress)->sin_addr, 0, 4);			
			}
		}else{
			memcpy(&((sockaddr_in*)clientAddress)->sin_addr, 0, 4);
		}
	}
	return iRet;
}

int select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, const struct timeval FAR * timeout)
{
	if (!readfds && !writefds && !exceptfds)
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	long long timeWait = -1;
	if (timeout)
		timeWait = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
	if (timeWait < 0)
	{
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}
	if (timeWait < 4)
		timeWait = 4;
	
	fd_set fdsRead;
	FD_ZERO(&fdsRead);
	if (readfds)
	{
		memcpy(&fdsRead, readfds, sizeof(fd_set));
		FD_ZERO(readfds);
	}

	fd_set fdsWrite;
	FD_ZERO(&fdsWrite);
	if (writefds)
	{
		memcpy(&fdsWrite, writefds, sizeof(fd_set));
		FD_ZERO(writefds);
	}

	fd_set fdsExcept;
	FD_ZERO(&fdsExcept);
	if (exceptfds)
	{
		memcpy(&fdsExcept, exceptfds, sizeof(fd_set));
		FD_ZERO(exceptfds);
	}

	int cnt = 0;
	long long startTime = GetTickCount64();
	long long elapsedTime = 0;
	do {
		if (readfds)
		{
			for (u_int i = 0; i < fdsRead.fd_count; i++)
			{
				int fd = fdsRead.fd_array[i];
				if (IsValidSock(fd))
				{
					if (g_ListWRTSocket[fd]->HasAsyncEvent(EVT_READ))
					{
						FD_SET(fd, readfds);
					}
				}
			}
			cnt += readfds->fd_count;
		}

		if (writefds)
		{
			for (u_int i = 0; i < fdsWrite.fd_count; i++)
			{
				int fd = fdsWrite.fd_array[i];
				if (IsValidSock(fd))
				{
					if (g_ListWRTSocket[fd]->HasAsyncEvent(EVT_WRITE))
					{
						FD_SET(fd, writefds);
					}
				}
			}
			cnt += writefds->fd_count;
		}

		if (exceptfds)
		{
			for (u_int i = 0; i < fdsExcept.fd_count; i++)
			{
				int fd = fdsExcept.fd_array[i];
				if (IsValidSock(fd))
				{
					if (g_ListWRTSocket[fd]->HasAsyncEvent(EVT_EXCEPT))
					{
						FD_SET(fd, exceptfds);
					}
				}
			}
			cnt += exceptfds->fd_count;
		}

		if (cnt > 0)
			return cnt;

		elapsedTime = GetTickCount64() - startTime;
		if (elapsedTime < timeWait)
		{
			Sleep(1);
			elapsedTime += 1;
		}
	} while(elapsedTime < timeWait);

	if (elapsedTime >= timeWait )
	{
		return 0;
	}
	return SOCKET_ERROR;
}

int socket(int protocolFamily, int type, int protocol)
{
	if (wsa_startup_count <= 0)
	{
		WSASetLastError(WSANOTINITIALISED);
		return SOCKET_ERROR;
	}
		
	int iRet = 0;
	for (; iRet < MAX_NUM_SOCKET; iRet++)
	{
		if (g_ListWRTSocket[iRet] == NULL)
			break;
	}

	if (iRet < MAX_NUM_SOCKET)
	{
		ISocketBase *socket = NULL;
		if(type == SOCK_DGRAM)
			socket = new UDPSocket();
		else
			socket = new TCPSocket();

		if (socket != NULL)
		{
			g_ListWRTSocket[iRet] = socket;
			return iRet;
		}
	}

	WSASetLastError(WSAEMFILE);
	return SOCKET_ERROR;	
}

HRESULT WRTSockToWSAError(SocketErrorStatus status)
{
	switch (status)
	{
	case SocketErrorStatus::AddressAlreadyInUse:
		return WSAEADDRINUSE;
	case SocketErrorStatus::AddressFamilyNotSupported:
		return WSAEAFNOSUPPORT;
	case SocketErrorStatus::CannotAssignRequestedAddress:
		return WSAEADDRNOTAVAIL;
	case SocketErrorStatus::ConnectionRefused:
		return WSAECONNREFUSED;
	case SocketErrorStatus::ConnectionResetByPeer:
		return WSAENETRESET;
	case SocketErrorStatus::ConnectionTimedOut:
		return WSAETIMEDOUT;
	case SocketErrorStatus::HostIsDown:
		return WSAEHOSTDOWN;
	case SocketErrorStatus::HostNotFound:
		return WSAHOST_NOT_FOUND;
	case SocketErrorStatus::MessageTooLong:
		return WSAEMSGSIZE;
	case SocketErrorStatus::NetworkDroppedConnectionOnReset:
		return WSAENETRESET;
	case SocketErrorStatus::NetworkIsDown:
		return WSAENETDOWN;
	case SocketErrorStatus::NetworkIsUnreachable:
		return WSAENETUNREACH;
	case SocketErrorStatus::NoAddressesFound:
		return WSAENOTCONN;
	case SocketErrorStatus::OperationAborted:
		return ERROR_OPERATION_ABORTED;
	case SocketErrorStatus::SocketTypeNotSupported:
		return WSAESOCKTNOSUPPORT;
	case SocketErrorStatus::TooManyOpenFiles:
		return WSAEMFILE;
	case SocketErrorStatus::SoftwareCausedConnectionAbort:
		return WSAECONNABORTED;
	case SocketErrorStatus::UnreachableHost:
		return WSAEHOSTUNREACH;	
	case SocketErrorStatus::Unknown:
	default:
		return 0;
	}
}

void ListenerContext::OnConnection(StreamSocketListener^ listener, StreamSocketListenerConnectionReceivedEventArgs^ object) 
{
	EnterCriticalSection(&lock);
	if (m_listener)
		m_listener->AddQueueConnection(object->Socket);
	LeaveCriticalSection(&lock);
}

void ListenerContext::OnMessageRecv(DatagramSocket^ listener, DatagramSocketMessageReceivedEventArgs^ object) 
{
	EnterCriticalSection(&lock);
	if (m_listener)
		m_listener->AddQueueUDPMessage(object);
	LeaveCriticalSection(&lock);
}

ISocketBase::ISocketBase(void)
{
	m_Writer = nullptr;
	m_Reader = nullptr;
	m_IsNonBlocking = true;
	m_NonBlockEvent = NONE;
	m_AsyncState = IDLE;
	m_SocketType = NONETYPE;
	m_RecvBlockSize = 0;
}

inline void ISocketBase::SetAsyncState(AsyncState state, bool set)
{
	if (set)
		m_AsyncState = (m_AsyncState | state);
	else
		m_AsyncState = (m_AsyncState & (~state));	
#ifdef UPDATE_WSA_BY_THREAD
	if (m_AsyncState == IDLE)
		SetEvent(g_UpdateSocketPendingEvt);
#endif
}


ISocketBase::~ISocketBase(void)
{
}


TCPSocket::TCPSocket(void):ISocketBase()
{
	m_BindAddr = "";
	m_BindPort = -1;
	m_MaxConn = 0;
	m_NoDealy = false;
	m_KeepAlive = false;
	m_Socket = ref new StreamSocket();
	m_SocketType = TCP_CLIENT;
	m_listenerCtx = nullptr;
	m_RecvBuffersTotalSize = 0;
	m_SendBuffersTotalSize = 0;
}

TCPSocket::TCPSocket(StreamSocket^ client)
{
	m_BindAddr = "";
	m_BindPort = -1;
	m_MaxConn = 0;
	m_NoDealy = false;
	m_KeepAlive = false;
	m_Socket = client;
	m_SocketType = TCP_CLIENT;
	m_listenerCtx = nullptr;
	m_RecvBuffersTotalSize = 0;
	m_SendBuffersTotalSize = 0;

	if (m_Socket)
	{
		//this socket is connected
		m_Writer = ref new DataWriter(m_Socket->OutputStream);
		m_Reader = ref new DataReader(m_Socket->InputStream);
		LoadBufferLoop();
	}
}

void TCPSocket::LoadBufferLoop()
{
	if (IsConnected())
	{
		SetAsyncState(LOADING);
		m_Reader->InputStreamOptions = InputStreamOptions::Partial;
		task<unsigned int> preLoadTask = create_task(m_Reader->LoadAsync(MAX_TCP_SEGMENT_SIZE), m_reading_cts.get_token());
		preLoadTask.then([=](task<unsigned int> preLoadTaskRes)
		{
			bool keep_going = false;			
			try {
				preLoadTaskRes.get();
				unsigned int readSize = m_Reader->UnconsumedBufferLength;
				if (readSize > 0)
				{					
					auto bytes = ref new Array<unsigned char>(readSize);
					if (bytes != nullptr)
					{
						lock_guard<mutex> lock(m_RecvBuffMtx);
						m_Reader->ReadBytes(bytes);
						m_RecvBuffersTotalSize += readSize;

						StreamingBuffer^ streamMsg = ref new StreamingBuffer(bytes);
						m_RecvBuffers.push_back(streamMsg);

						SetAsyncEvent(EVT_READ, (m_RecvBuffersTotalSize > m_RecvBlockSize));						
					}

					if (m_RecvBuffersTotalSize < MAX_TCP_BUFFER_SIZE)
					{
						LoadBufferLoop();
						keep_going = true;
					}
				}				
			}
			catch(const task_canceled& e)
			{					
				WRTSOCK_DBG_INFO("Reading was canceled. Message[%s]", e.what());
			}
			catch(Exception^ ex)
			{					
				SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
				switch(errcode)
				{
				case SocketErrorStatus::ConnectionRefused:
				case SocketErrorStatus::ConnectionResetByPeer:
				case SocketErrorStatus::ConnectionTimedOut:
				case SocketErrorStatus::HostIsDown:
				case SocketErrorStatus::SoftwareCausedConnectionAbort:
					SetAsyncEvent(EVT_READ);
					break;
				default:
					break;
				}
				WSASetLastError(WRTSockToWSAError(errcode));
				WRTSOCK_DBG_ERROR("Reading failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
				closesocket(false);
			}
			if (!keep_going)
				SetAsyncState(LOADING, false);
		});
		Sleep(1);
	}	
}

TCPSocket::~TCPSocket(void)
{
	//Do not delete the socket here. It must be deleted by closing the socket.
	//And waiting until all async function are done, we could delete it.
	closesocket();
}

int TCPSocket::listen(int backlog)
{
	if (m_Socket)
	{
		//this is not useful
		delete m_Socket;
		m_Socket = nullptr;
	}

	m_SocketListener = ref new StreamSocketListener();
	m_MaxConn = backlog;
	m_SocketType = TCP_SERVER;	
	HostName^ localhost = nullptr;
	if (m_BindAddr != "0.0.0.0" && m_BindAddr != "localhost")
		localhost = ref new HostName(stops(m_BindAddr));	
	m_listenerCtx = ref new ListenerContext(this);

	m_SocketListener->ConnectionReceived +=  ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs^>(m_listenerCtx, &ListenerContext::OnConnection);
	task<void> bindTask = create_task(m_SocketListener->BindEndpointAsync(localhost, m_BindPort.ToString()));
	try 
	{		
		// Try getting all exceptions from the continuation chain above this point.
		bindTask.get();
		WRTSOCK_DBG_INFO("Listening on [%s:%d] success!\n", getlocaladdr().c_str(), getlocalport()); 
	} 
	catch (Exception^ ex) 
	{
		SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
		WSASetLastError(WRTSockToWSAError(errcode));
		WRTSOCK_DBG_ERROR("Bind to [%s:%d] failed. ErrorCode [%d], ErrorMessage: %s", m_BindAddr.c_str(), m_BindPort, errcode, pstos(ex->Message).c_str());
	}
	return 0;
}

int TCPSocket::accept()
{
	if (m_SocketListener == nullptr || m_ClientQueue.size() <= 0)
	{
		WSASetLastError(WSAEINVAL);
		return INVALID_SOCKET;
	}
	
	int fd = 0;
	for (; fd < MAX_NUM_SOCKET; fd++)
	{
		if (g_ListWRTSocket[fd] == NULL)
			break;
	}
	
	if (fd < MAX_NUM_SOCKET)
	{
		TCPSocket *socket = new TCPSocket(m_ClientQueue.front());
		m_ClientQueue.erase(m_ClientQueue.begin());
		g_ListWRTSocket[fd] = socket;
		return fd;
	}

	//exceed the maximum of number socket
	delete m_ClientQueue.front();
	m_ClientQueue.erase(m_ClientQueue.begin());
	WSASetLastError(WSAEMFILE);
	return INVALID_SOCKET;
}

int TCPSocket::connect(string dstaddr, int dstport)
{
	if (m_IsNonBlocking)
	{
		if (IsDoingAsync(CONNECTING))
		{
			WSASetLastError(WSAEALREADY);
			return SOCKET_ERROR;
		}
		String^ DstIP = stops(dstaddr);
		String^ DstPort = dstport.ToString();
		HostName^ DstName = ref new HostName(DstIP);
		
		//set control
		//{
		//	m_Socket->Control->NoDelay = true;//m_NoDealy;
		//	m_Socket->Control->KeepAlive = true;//m_KeepAlive;
		//}

		SetAsyncState(CONNECTING);
		task<void> connectTask = create_task(m_Socket->ConnectAsync(DstName, DstPort, SocketProtectionLevel::PlainSocket), m_connecting_cts.get_token());
		connectTask.then([=](task<void> connectTaskRes)				
		{
			try {
				connectTaskRes.get();
				if (m_Socket != nullptr && m_Socket->OutputStream != nullptr)
				{
					m_Writer = ref new DataWriter(m_Socket->OutputStream);
					m_Reader = ref new DataReader(m_Socket->InputStream);
					SetAsyncEvent(EVT_WRITE);
					WRTSOCK_DBG_INFO("Connecting from [localhost:%d] to [%s:%d] success.", getlocalport(), getremoteaddr().c_str(), getremoteport());			
					LoadBufferLoop();
				}
			}
			catch(const task_canceled& e)
			{					
				WRTSOCK_DBG_INFO("Connecting was canceled. Message[%s]", e.what());
			}
			catch(Exception^ ex)
			{					
				closesocket(false);
				SetAsyncEvent(EVT_EXCEPT);
				SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
				WSASetLastError(WRTSockToWSAError(errcode));
				WRTSOCK_DBG_ERROR("Connecting to [%s:%s] failed. ErrorCode [%d], ErrorMessage: %s", pstos(DstIP).c_str(), pstos(DstPort).c_str() , errcode, pstos(ex->Message).c_str());
			}
			SetAsyncState(CONNECTING, false);
		});					
		Sleep(1);

		WSASetLastError(WSAEWOULDBLOCK);
		return SOCKET_ERROR;		
	}

	return 0;
}

void TCPSocket::UpLoadBufferLoop()
{
	bool needflush = false;
	if (m_SendBuffersTotalSize > 0 && m_SendBuffers.size() > 0)
	{
		lock_guard<mutex> lock(m_SendBuffMtx);
		StreamingBuffer^ wBuffer = m_SendBuffers[0];
		int unstoredsize = wBuffer->GetUnConsumedSize();
		auto bytes = ref new Array<unsigned char>(unstoredsize);
		wBuffer->ReadNoSeek(bytes->Data, unstoredsize);

		m_Writer->WriteBytes(bytes);
		needflush = true;
	}

	if (needflush)
	{
		SetAsyncState(STORING);
		task<unsigned int> sendingTask = create_task(m_Writer->StoreAsync(), m_writing_cts.get_token());
		sendingTask.then([=](task<unsigned int> sendingTaskRes)
		{
			bool keep_going = false;
			try {
				size_t wroteSize = sendingTaskRes.get();

				if (wroteSize > 0)
				{
					lock_guard<mutex> lock(m_SendBuffMtx);
					m_SendBuffersTotalSize -= wroteSize;
					StreamingBuffer^ wBuffer = m_SendBuffers[0];
					wBuffer->Seek(wroteSize);
					if (wBuffer->GetUnConsumedSize() <= 0)
						m_SendBuffers.erase(m_SendBuffers.begin());					
				}
				if (m_SendBuffersTotalSize > 0)
				{
					UpLoadBufferLoop();
					keep_going = true;
				}

				SetAsyncEvent(EVT_WRITE, m_SendBuffersTotalSize <= MAX_TCP_BUFFER_SIZE);				
			}
			catch(const task_canceled& e)
			{					
				WRTSOCK_DBG_INFO("Writing was canceled. Message[%s]", e.what());
			}
			catch(Exception^ ex)
			{				
				SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
				switch(errcode)
				{
				case SocketErrorStatus::ConnectionRefused:
				case SocketErrorStatus::ConnectionResetByPeer:
				case SocketErrorStatus::ConnectionTimedOut:
				case SocketErrorStatus::HostIsDown:
				case SocketErrorStatus::SoftwareCausedConnectionAbort:
					SetAsyncEvent(EVT_READ);
					break;
				default:
					break;
				}
				WSASetLastError(WRTSockToWSAError(errcode));
				WRTSOCK_DBG_ERROR("Writing failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
				closesocket(false);
			}
			if (!keep_going)
				SetAsyncState(STORING, false);
		});
		Sleep(1);
	}
}

int TCPSocket::send(const char *buf, int len)
{
	if (m_IsNonBlocking)
	{		
		if (!IsConnected())
		{
			WSASetLastError(WSAENOTCONN);
			return SOCKET_ERROR;
		}
		if (len == 0)
			return 0;

		auto bytes = ref new Array<unsigned char>(len);
		memcpy(bytes->Data, buf, len);
		StreamingBuffer^ wBuffer = ref new StreamingBuffer(bytes);
		if  (wBuffer != nullptr)
		{
			lock_guard<mutex> lock(m_SendBuffMtx);
			m_SendBuffers.push_back(wBuffer);
			m_SendBuffersTotalSize += len;
		}

		if (!IsDoingAsync(STORING))
		{
			UpLoadBufferLoop();
		}


		SetAsyncEvent(EVT_WRITE, m_SendBuffersTotalSize <= MAX_TCP_BUFFER_SIZE);

		return len;
	}
	return 0;
}

int TCPSocket::wsasendto(LPWSABUF lpBuffers, DWORD dwBufferCount, string dstaddr, int port)
{
	WRTSOCK_DBG_ERROR("Not implemented!!!");
	return 0;
}

int TCPSocket::recv(char *buf, int len, int flags)
{
	if (m_IsNonBlocking)
	{		
		if (!IsConnected() || m_Socket == nullptr)
		{
			WSASetLastError(WSAENOTCONN);
			return SOCKET_ERROR;
		}

		size_t readSize = 0;
		if (m_RecvBuffersTotalSize > 0 && m_RecvBuffers.size() > 0)
		{
			lock_guard<mutex> lock(m_RecvBuffMtx);
			StreamingBuffer^ streamMsg = m_RecvBuffers[0];


			while(readSize < len)
			{
				int unreadSize = len - readSize;
				readSize += streamMsg->Read((unsigned char*)(buf+readSize), unreadSize);				

				if (streamMsg->GetUnConsumedSize() <= 0)
					m_RecvBuffers.erase(m_RecvBuffers.begin());

				if (m_RecvBuffers.size() > 0)
					streamMsg = m_RecvBuffers[0];
				else
					break;
			}
			m_RecvBuffersTotalSize -= readSize;
		}		

		if (m_RecvBuffersTotalSize < MAX_TCP_BUFFER_SIZE)
		{			
			if (!IsDoingAsync(LOADING))
				LoadBufferLoop();
		}
		
		SetAsyncEvent(EVT_READ, (m_RecvBuffersTotalSize > m_RecvBlockSize));

		if (readSize <= 0)
		{
			WSASetLastError(WSAEWOULDBLOCK);
			return SOCKET_ERROR;
		}
		return readSize;
	}
	return 0;
}

int TCPSocket::recvfrom(char *buf, int len, int flags, string &fromIp, int &fromPort)
{
	fromIp = getremoteaddr();
	fromPort = getremoteport();
	return recv(buf, len, flags);
}

int TCPSocket::wsarecvfrom(LPWSABUF lpBuffers, DWORD dwBufferCount, string &dstaddr, int &port)
{
	WRTSOCK_DBG_ERROR("Not implemented!!!");
	return 0;
}

int TCPSocket::closesocket(bool needcancel)
{
	if (needcancel)
	{
		m_connecting_cts.cancel();
		m_reading_cts.cancel();
		m_writing_cts.cancel();
	}
	if (m_Socket)
	{
		delete m_Socket;
		m_Socket = nullptr;
	}

	if (m_SocketListener)
	{
		delete m_SocketListener;
		m_SocketListener = nullptr;
	}
	m_RecvBuffMtx.lock();
	m_RecvBuffMtx.unlock();
	m_SendBuffMtx.lock();
	m_SendBuffMtx.unlock();
	return 0;
}

string TCPSocket::getlocaladdr()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->LocalAddress != nullptr)
	{
		String^ localAddr = m_Socket->Information->LocalAddress->DisplayName;
		if (localAddr != nullptr)
		{
			std::string localIPv4 = pstos(localAddr);
			if (GetNetAddressType(localIPv4.c_str()) == 1)
				return localIPv4;
		}
	}
	char localip[128] = {0};
	if(getlocalip(localip, 128)==0)
		return std::string(localip);
	return "";
}

int TCPSocket::getlocalport()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->LocalPort != nullptr)
	{
		String^ localport = m_Socket->Information->LocalPort;
		return atoi(pstos(localport).c_str());
	}
	else if (m_SocketListener != nullptr && m_SocketListener->Information != nullptr && m_SocketListener->Information->LocalPort != nullptr)
	{
		String^ localport = m_SocketListener->Information->LocalPort;
		return atoi(pstos(localport).c_str());
	}
	return -1;
}

string TCPSocket::getremoteaddr()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->RemoteAddress != nullptr)
	{
		String^ remoteAddr = m_Socket->Information->RemoteAddress->DisplayName;
		if (remoteAddr != nullptr)
			return pstos(remoteAddr);
	}
	return "";
}

int TCPSocket::getremoteport()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->RemotePort != nullptr)
	{
		String^ remoteport = m_Socket->Information->RemotePort;
		return atoi(pstos(remoteport).c_str());
	}
	return -1;
}

int TCPSocket::setsockopt(int level, int optname, const char* optval, int optlen)
{
	WRTSOCK_DBG_ERROR("Not implemented!!!");
	return SOCKET_ERROR;
}

int TCPSocket::getsockopt(int level, int optname, char* optval, int *optlen)
{
	WRTSOCK_DBG_ERROR("Not implemented!!!");
	return SOCKET_ERROR;
}

void TCPSocket::AddQueueConnection(StreamSocket^ client)
{
	if (m_ClientQueue.size() >= m_MaxConn)
	{
		delete client;
		return;
	}	
	m_ClientQueue.push_back(client);
}

void TCPSocket::AddQueueUDPMessage(DatagramSocketMessageReceivedEventArgs^ clientEvent)
{
	WRTSOCK_DBG_ERROR("This method only used for UDP Socket!!!");
}

UDPSocket::UDPSocket(void)
{
	m_Socket = ref new DatagramSocket();
	m_MsgQueue.clear();
	m_UnConsumedBufferLen = 0;
	m_ConnectedAddr = "";
	m_ConnectedPort = 0;
}

int UDPSocket::bind(string addr, int port)
{	
	HostName^ localhost = nullptr;
	if (addr != "0.0.0.0" && addr != "localhost")
		localhost = ref new HostName(stops(addr));
	m_listenerCtx = ref new ListenerContext(this); 
	
	m_Socket->MessageReceived +=  ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(m_listenerCtx, &ListenerContext::OnMessageRecv);
	task<void> bindTask = create_task(m_Socket->BindEndpointAsync(localhost, port.ToString()));
	try 
	{ 		
		// Try getting all exceptions from the continuation chain above this point.
		bindTask.get();
		WRTSOCK_DBG_INFO("UDP Bound to [%s:%d] success!\n", getlocaladdr().c_str(), getlocalport()); 
	} 
	catch (Exception^ ex) 
	{
		SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
		WSASetLastError(WRTSockToWSAError(errcode));
		WRTSOCK_DBG_ERROR("UDP Bind to [%s:%d] failed. ErrorCode [%d], ErrorMessage: %s", addr.c_str(), port, errcode, pstos(ex->Message).c_str());
	}
	return 0;
}

int UDPSocket::listen(int backlog)
{
	WRTSOCK_DBG_ERROR("This method only used for TCP Server Socket!!!");
	return SOCKET_ERROR;
}

int UDPSocket::accept()
{
	WRTSOCK_DBG_ERROR("This method only used for TCP Server Socket!!!");
	return SOCKET_ERROR;
}

int UDPSocket::connect(string dstaddr, int dstport)
{
	if (dstaddr == "")
	{
		WRTSOCK_DBG_INFO("UDP disconnected session to [%s:%d]", m_ConnectedAddr.c_str(), m_ConnectedPort);
		m_ConnectedAddr = "";
		m_ConnectedPort = 0;
		m_Writer = nullptr;		
		return 0;
	}
	if (m_IsNonBlocking)
	{
		String^ DstIP = stops(dstaddr);
		String^ DstPort = dstport.ToString();
		HostName^ DstName = ref new HostName(DstIP);
		
		//TODO: set control

		SetAsyncState(CONNECTING);
		task<void> connectTask = create_task(m_Socket->ConnectAsync(DstName, DstPort), m_connecting_cts.get_token());
		connectTask.then([=](task<void> connectTaskRes)				
		{
			try {
				connectTaskRes.get();
				m_Writer = ref new DataWriter(m_Socket->OutputStream);
				m_ConnectedAddr = dstaddr;
				m_ConnectedPort = dstport;
				SetAsyncEvent(EVT_WRITE);
				WRTSOCK_DBG_INFO("UDP Connecting from [localhost:%d] to [%s:%d] success.", getlocalport(), getremoteaddr().c_str(), getremoteport());												
			}
			catch(const task_canceled& e)
			{					
				WRTSOCK_DBG_INFO("UDP Connecting was canceled. Message[%s]", e.what());
			}
			catch(Exception^ ex)
			{				
				SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
				SetAsyncEvent(EVT_EXCEPT);
				WSASetLastError(WRTSockToWSAError(errcode));
				WRTSOCK_DBG_ERROR("UDP Connecting to [%s:%s] failed. ErrorCode [%d], ErrorMessage: %s", pstos(DstIP).c_str(), pstos(DstPort).c_str() , errcode, pstos(ex->Message).c_str());
				closesocket(false);
			}
			SetAsyncState(CONNECTING, false);
		});					

		WSASetLastError(WSAEWOULDBLOCK);
		return SOCKET_ERROR;		
	}

	return 0;
}

int UDPSocket::send(const char *buf, int len)
{
	if (m_ConnectedAddr == "")
	{
		WSASetLastError(WSAENOTCONN);
		return SOCKET_ERROR;
	}

	if (m_IsNonBlocking)
	{
		if (IsDoingAsync(STORING))
		{
			WSASetLastError(WSAEALREADY);
			return SOCKET_ERROR;
		}
		if (m_ConnectedAddr == "")
		{
			WSASetLastError(WSAENOTCONN);
			return SOCKET_ERROR;
		}
		if (len == 0)
			return 0;
						
		SetAsyncEvent(EVT_WRITE, false);
		auto bytes = ref new Array<unsigned char>((unsigned char*)buf, len);
		m_Writer->WriteBytes(bytes);

		SetAsyncState(STORING);
		task<unsigned int> sendingTask = create_task(m_Writer->StoreAsync(), m_writing_cts.get_token());
		sendingTask.then([=](task<unsigned int> sendingTaskRes)
		{
			try {
				size_t wroteSize = sendingTaskRes.get();
				if (wroteSize != len)
				{
					WRTSOCK_DBG_WARNING("UDP Writing may be corrupted. Wrote [%d], Expected [%d]", wroteSize, len);
				}
				WRTSOCK_DBG_INFO("UDP Sent from [localhost:%d] to [%s:%d] Len [%d]", getlocalport(), getremoteaddr().c_str(), getremoteport(), wroteSize);
				SetAsyncEvent(EVT_WRITE);
			}
			catch(const task_canceled& e)
			{					
				WRTSOCK_DBG_INFO("UDP Writing was canceled. Message[%s]", e.what());
			}
			catch(Exception^ ex)
			{				
				SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
				switch(errcode)
				{
				case SocketErrorStatus::ConnectionRefused:
				case SocketErrorStatus::ConnectionResetByPeer:
				case SocketErrorStatus::ConnectionTimedOut:
				case SocketErrorStatus::HostIsDown:
				case SocketErrorStatus::SoftwareCausedConnectionAbort:
					SetAsyncEvent(EVT_READ);
					break;
				default:
					break;
				}
				WSASetLastError(WRTSockToWSAError(errcode));
				WRTSOCK_DBG_ERROR("UDP Writing failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
				WRTSOCK_DBG_ERROR("UDP Sent from [localhost:%d] to [%s:%d] Len[%d]", getlocalport(), getremoteaddr().c_str(), getremoteport(), len);
				closesocket(false);

			}
			SetAsyncState(STORING, false);
		});
		return len;
	}
	return 0;
}

int UDPSocket::sendto(const char *buf, int len, string dstaddr, int port)
{
	try {

		if (m_ConnectedAddr != dstaddr || m_ConnectedPort != port || m_Writer == nullptr)
		{			
			m_ConnectedAddr = dstaddr;
			m_ConnectedPort = port;
			String^ remoteHost = stops(m_ConnectedAddr);
			auto getStreamTask = create_task(m_Socket->GetOutputStreamAsync(ref new HostName(remoteHost), port.ToString()));
			IOutputStream^ stream = getStreamTask.get();
			if (stream)
				m_Writer = ref new DataWriter(stream);
		}

		if (m_IsNonBlocking)
		{			
			if (IsDoingAsync(STORING))
			{
				WSASetLastError(WSAEALREADY);
				return SOCKET_ERROR;
			}

			if (len == 0)
				return 0;
						
			SetAsyncEvent(EVT_WRITE, false);
			auto bytes = ref new Array<unsigned char>((unsigned char*)buf, len);
			m_Writer->WriteBytes(bytes);

			SetAsyncState(STORING);
			task<unsigned int> sendingTask = create_task(m_Writer->StoreAsync(), m_writing_cts.get_token());
			sendingTask.then([=](task<unsigned int> sendingTaskRes)
			{
				try {
					size_t wroteSize = sendingTaskRes.get();
					if (wroteSize != len)
					{
						WRTSOCK_DBG_WARNING("UDP Writing may be error. Wrote [%d], Expected [%d]", wroteSize, len);
					}
					WRTSOCK_DBG_INFO("UDP Sent from [localhost:%d] to [%s:%d] Len [%d]", getlocalport(), getremoteaddr().c_str(), getremoteport(), wroteSize);
				}
				catch(const task_canceled& e)
				{					
					WRTSOCK_DBG_INFO("UDP Writing was canceled. Message[%s]", e.what());
				}
				catch(Exception^ ex)
				{				
					SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
					WSASetLastError(WRTSockToWSAError(errcode));
					WRTSOCK_DBG_ERROR("UDP Writing failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
					WRTSOCK_DBG_ERROR("UDP Sent from [localhost:%d] to [%s:%d] Len[%d]", getlocalport(), getremoteaddr().c_str(), getremoteport(), len);
					closesocket(false);
				}
				SetAsyncEvent(EVT_WRITE);
				SetAsyncState(STORING, false);
			});
			return len;
		}
	} catch (Exception^ ex)
	{		
		SetAsyncEvent(EVT_WRITE);
		SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
		WRTSOCK_DBG_ERROR("UDP Get output stream failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
		WRTSOCK_DBG_ERROR("UDP Sent from [localhost:%d] to [%s:%d]", getlocalport(), dstaddr.c_str(), port);
		closesocket(false);
	}
	return 0;
}

int UDPSocket::wsasendto(LPWSABUF lpBuffers, DWORD dwBufferCount, string dstaddr, int port)
{
	try {

		if (m_ConnectedAddr != dstaddr || m_ConnectedPort != port || m_Writer == nullptr)
		{			
			m_ConnectedAddr = dstaddr;
			m_ConnectedPort = port;
			String^ remoteHost = stops(m_ConnectedAddr);
			auto getStreamTask = create_task(m_Socket->GetOutputStreamAsync(ref new HostName(remoteHost), port.ToString()));
			IOutputStream^ stream = getStreamTask.get();
			if (stream)
				m_Writer = ref new DataWriter(stream);
		}

		if (m_IsNonBlocking)
		{			
			if (IsDoingAsync(STORING))
			{
				WSASetLastError(WSAEALREADY);
				return SOCKET_ERROR;
			}

			int len = 0;
			for (int i = 0; i < dwBufferCount; i++)
			{
				len += lpBuffers[i].len;
			}

			if (len == 0)
				return 0;
						
			SetAsyncEvent(EVT_WRITE, false);
			auto bytes = ref new Array<unsigned char>(len);
			int offset = 0;
			for (int i = 0; i < dwBufferCount; i++)
			{
				memcpy(bytes->Data+offset, lpBuffers[i].buf, lpBuffers[i].len);
				offset += lpBuffers[i].len;
			}
			m_Writer->WriteBytes(bytes);

			SetAsyncState(STORING);
			task<unsigned int> sendingTask = create_task(m_Writer->StoreAsync(), m_writing_cts.get_token());
			sendingTask.then([=](task<unsigned int> sendingTaskRes)
			{
				try {
					size_t wroteSize = sendingTaskRes.get();
					if (wroteSize != len)
					{
						WRTSOCK_DBG_WARNING("UDP Writing may be error. Wrote [%d], Expected [%d]", wroteSize, len);
					}
					WRTSOCK_DBG_INFO("UDP Sent from [localhost:%d] to [%s:%d] Len [%d]", getlocalport(), getremoteaddr().c_str(), getremoteport(), wroteSize);
				}
				catch(const task_canceled& e)
				{					
					WRTSOCK_DBG_INFO("UDP Writing was canceled. Message[%s]", e.what());
				}
				catch(Exception^ ex)
				{				
					SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
					WSASetLastError(WRTSockToWSAError(errcode));
					WRTSOCK_DBG_ERROR("UDP Writing failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
					WRTSOCK_DBG_ERROR("UDP Sent from [localhost:%d] to [%s:%d] Len[%d]", getlocalport(), getremoteaddr().c_str(), getremoteport(), len);
					closesocket(false);
				}
				SetAsyncEvent(EVT_WRITE);
				SetAsyncState(STORING, false);
			});
			return len;
		}
	} catch (Exception^ ex)
	{		
		SetAsyncEvent(EVT_WRITE);
		SocketErrorStatus errcode = SocketError::GetStatus(ex->HResult);
		WRTSOCK_DBG_ERROR("UDP Get output stream failed. ErrorCode [%d], ErrorMessage: %s", errcode, pstos(ex->Message).c_str());
		WRTSOCK_DBG_ERROR("UDP Sent from [localhost:%d] to [%s:%d]", getlocalport(), dstaddr.c_str(), port);
		closesocket(false);
	}
	return 0;
}

int UDPSocket::recv(char *buf, int len, int flags)
{
	lock_guard<mutex> lock(m_MsgMtx);
	if (m_ConnectedAddr == "")
	{
		WSASetLastError(WSAENOTCONN);
		return SOCKET_ERROR;
	}
	int readsize = 0;
	if (m_UnConsumedBufferLen > 0)
	{			

		DatagramMessage^ dgramMsg = m_MsgQueue.front();
		String^ remoteHost = dgramMsg->RemoteAddr;
		String^ remotePort = dgramMsg->RemotePort;
		int unread = len;
		while(true)
		{
			string remoteAddr = pstos(remoteHost);
			int iRemotePort = atoi(pstos(remotePort).c_str());
			DataReader^ reader = dgramMsg->Reader;
			if (remoteAddr == m_ConnectedAddr && iRemotePort == m_ConnectedPort)
			{
				int unconsumed = reader->UnconsumedBufferLength;
				int bytesize = (unread<unconsumed?unread:unconsumed);

				auto bytes = ref new Array<unsigned char>(bytesize);
				reader->ReadBytes(bytes);
				memcpy(buf+readsize, bytes->Data, bytesize);
				readsize += bytesize;			
				unread -= bytesize;
				m_MsgQueue.erase(m_MsgQueue.begin());
				m_UnConsumedBufferLen -= unconsumed;

				if (unread <= 0 || m_UnConsumedBufferLen <= 0)
					break;
			}
			else
			{
				m_UnConsumedBufferLen -= reader->UnconsumedBufferLength;
				m_MsgQueue.erase(m_MsgQueue.begin());
			}
			
			if (m_UnConsumedBufferLen <= 0)
				break;

			dgramMsg = m_MsgQueue.front();
			remoteHost = dgramMsg->RemoteAddr;
			remotePort = dgramMsg->RemotePort;
		}			
	}
	
	SetAsyncEvent(EVT_READ, (m_UnConsumedBufferLen > m_RecvBlockSize));

	return readsize;
}

int UDPSocket::recvfrom(char *buf, int len, int flags, string &fromIp, int &fromPort)
{
	lock_guard<mutex> lock(m_MsgMtx);
	int readsize = 0;
	if (m_UnConsumedBufferLen > 0)
	{			
		DatagramMessage^ dgramMsg = m_MsgQueue.front();
		String^ remoteHost = dgramMsg->RemoteAddr;
		String^ remotePort = dgramMsg->RemotePort;
		int unread = len;		

		while(true)
		{
			DataReader^ reader = dgramMsg->Reader;
			int unconsumed = reader->UnconsumedBufferLength;
			int bytesize = (unread<unconsumed?unread:unconsumed);

			auto bytes = ref new Array<unsigned char>(bytesize);
			reader->ReadBytes(bytes);			
			memcpy(buf+readsize, bytes->Data, bytesize);
			readsize += bytesize;			
			unread -= bytesize;
			m_MsgQueue.erase(m_MsgQueue.begin());
			m_UnConsumedBufferLen -= unconsumed;

			if (unread <= 0 || m_UnConsumedBufferLen <= 0)
				break;
			
			dgramMsg = m_MsgQueue.front();
			if (dgramMsg == nullptr || remoteHost != dgramMsg->RemoteAddr || remotePort != dgramMsg->RemotePort)
				break;			
		}

		if (remoteHost != nullptr)
			fromIp = pstos(remoteHost);

		if (remotePort != nullptr && remotePort->Length() > 0)
			fromPort = atoi(pstos(remotePort).c_str());				
	}
	else
	{
		fromPort = 0;
		fromIp = "";		
	}

	SetAsyncEvent(EVT_READ, (m_UnConsumedBufferLen > m_RecvBlockSize));

	return readsize;
}

int UDPSocket::wsarecvfrom(LPWSABUF lpBuffers, DWORD dwBufferCount, string &dstaddr, int &port)
{
	lock_guard<mutex> lock(m_MsgMtx);
	int readsize = 0;
	if (m_UnConsumedBufferLen > 0)
	{
		int currIdx = 0;
		DatagramMessage^ dgramMsg = m_MsgQueue[currIdx];
		String^ remoteHost = dgramMsg->RemoteAddr;
		String^ remotePort = dgramMsg->RemotePort;
		for(int i = 0; i < dwBufferCount; i++)
		{
			int unread = lpBuffers[i].len;
			DataReader^ reader = dgramMsg->Reader;
			int unconsumed = reader->UnconsumedBufferLength;
			int bytesize = (unread<unconsumed?unread:unconsumed);
			auto bytes = ref new Array<unsigned char>(bytesize);
			reader->ReadBytes(bytes);
			memcpy(lpBuffers[i].buf, bytes->Data, bytes->Length);
			readsize += bytes->Length;


			m_MsgQueue.erase(m_MsgQueue.begin() + currIdx);
			m_UnConsumedBufferLen -= unconsumed;

			if (m_UnConsumedBufferLen <= 0)
				break;

			
			do {
				dgramMsg =  m_MsgQueue[currIdx];			
				if (remoteHost == dgramMsg->RemoteAddr && remotePort == dgramMsg->RemotePort)
					break;
				currIdx++;
			} while (currIdx < m_MsgQueue.size());

			if (currIdx >= m_MsgQueue.size())
				break;
		}		

		if (remoteHost != nullptr)
			dstaddr = pstos(remoteHost);

		if (remotePort != nullptr && remotePort->Length() > 0)
			port = atoi(pstos(remotePort).c_str());				
	}
	else
	{
		port = 0;
		dstaddr = "";		
	}

	SetAsyncEvent(EVT_READ, (m_UnConsumedBufferLen > m_RecvBlockSize));

	return readsize;
}

int UDPSocket::closesocket(bool needcancel = true)
{
	if (needcancel)
	{
		m_connecting_cts.cancel();
		m_reading_cts.cancel();
		m_writing_cts.cancel();		
		if (m_Socket)
		{
			delete m_Socket;
			m_Socket = nullptr;
		}
	}
	else
	{
		m_Writer = nullptr;
	}
	return 0;
}

string UDPSocket::getlocaladdr()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->LocalAddress != nullptr)
	{
		String^ localAddr = m_Socket->Information->LocalAddress->DisplayName;
		if (localAddr != nullptr)
		{
			std::string localIPv4 = pstos(localAddr);
			if (GetNetAddressType(localIPv4.c_str()) == 1)
				return localIPv4;
		}
	}
	char localip[128] = {0};
	if(getlocalip(localip, 128)==0)
		return std::string(localip);
	return "";
}

int UDPSocket::getlocalport()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->LocalPort != nullptr)
	{
		String^ localport = m_Socket->Information->LocalPort;
		return atoi(pstos(localport).c_str());
	}	
	return -1;
}

string UDPSocket::getremoteaddr()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->RemoteAddress != nullptr)
	{
		String^ remoteAddr = m_Socket->Information->RemoteAddress->DisplayName;
		if (remoteAddr != nullptr)
			return pstos(remoteAddr);
	}
	if (m_ConnectedAddr != "")
	{
		return m_ConnectedAddr;
	}
	return "";
}

int UDPSocket::getremoteport()
{
	if (m_Socket != nullptr && m_Socket->Information != nullptr && m_Socket->Information->RemotePort != nullptr)
	{
		String^ remoteport = m_Socket->Information->RemotePort;
		return atoi(pstos(remoteport).c_str());
	}
	if (m_ConnectedPort > 0)
		return m_ConnectedPort;
	return -1;
}

int UDPSocket::setsockopt(int level, int optname, const char* optval, int optlen)
{
	WRTSOCK_DBG_ERROR("Not implemented!!!");
	return 0;
}

int UDPSocket::getsockopt(int level, int optname, char* optval, int *optlen)
{
	WRTSOCK_DBG_ERROR("Not implemented!!!");
	return 0;
}

void UDPSocket::AddQueueConnection(StreamSocket^ client)
{
	WRTSOCK_DBG_ERROR("This method only used for TCP Socket!!!");
}

void UDPSocket::AddQueueUDPMessage(DatagramSocketMessageReceivedEventArgs^ clientEvent)
{
	lock_guard<mutex> lock(m_MsgMtx);
	if (m_UnConsumedBufferLen < MAX_UDP_BUFFER_SIZE) {
		string remoteaddr = pstos(clientEvent->RemoteAddress->DisplayName);
		string localaddr = pstos(clientEvent->LocalAddress->DisplayName);		
		if (remoteaddr == localaddr)
			return;

		int remoteport = atoi(pstos(clientEvent->RemotePort).c_str());

		DataReader^ dtReader = clientEvent->GetDataReader();

		DatagramMessage^ dgramMessage = ref new DatagramMessage();	
		dgramMessage->Reader = dtReader;

		m_UnConsumedBufferLen += dgramMessage->Reader->UnconsumedBufferLength;
		dgramMessage->LocalAddr = clientEvent->LocalAddress->DisplayName;
		dgramMessage->RemoteAddr = clientEvent->RemoteAddress->DisplayName;
		dgramMessage->RemotePort = clientEvent->RemotePort;	


		m_MsgQueue.push_back(dgramMessage);
	}

	SetAsyncEvent(EVT_READ, (m_UnConsumedBufferLen > m_RecvBlockSize));
}


UDPSocket::~UDPSocket(void)
{
	closesocket();
}