#pragma once
#include <ppl.h>
#include <ppltasks.h>
#include <stdarg.h>
#include <thread>
#include <string>
#include <mutex>

#if defined OS_W8 || defined OS_WP8 || defined OS_W10
#include "metrosockdef.h"
#else
#include <winsock2.h>
#endif

#include "ThreadEmulation.h"
#include "WinRTStringUtils.h"

#ifdef _DEBUG
#define SOCK_DEBUG_OUT(...)                     __Debug_out(__FUNCTION__ , __LINE__, __VA_ARGS__)
#else
#define SOCK_DEBUG_OUT(...)
#endif
#define SOCK_API_DEL(a)			{ if(a) { delete (a); a=NULL; } }
#define SOCK_API_DELETE_ARRAY(a)	{ if(a) { delete [] (a); a=NULL; } }
#define SOCK_API_DEL_ARRAY(p, n)	{ if(p) { for (int __i = 0; __i < n;__i++) SOCK_API_DELETE_ARRAY((p)[__i]); delete[] (p); p = NULL; } }
char * SOCK_API_STRNEW( const char * string );
void __Debug_out(const char* pFunction, int lineNumber, const char* pFormat, ...);

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking::Sockets;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Networking::Connectivity;
using namespace Windows::System::Threading;
using namespace std;

enum AsyncState{
	IDLE = 0,
	CONNECTING = 1,
	LOADING = 2,
	STORING = 4,
};

enum NonBlockEvent{
	NONE = 0,
	EVT_READ = 1,
	EVT_WRITE = 2,
	EVT_EXCEPT = 4,
};

enum WRTSockType{
	NONETYPE = 0,
	UDP,
	TCP_CLIENT,
	TCP_SERVER	
};

class ISocketBase;
ref class ListenerContext sealed
{
public:        
    void OnConnection(StreamSocketListener^ listener, StreamSocketListenerConnectionReceivedEventArgs^ object);
	void OnMessageRecv(DatagramSocket^ listener, DatagramSocketMessageReceivedEventArgs^ object);
internal:
	ListenerContext(ISocketBase* listener){
		m_listener = listener;
		InitializeCriticalSectionEx(&lock, 0, 0);
	}
private:
	~ListenerContext(){
		DeleteCriticalSection(&lock);
	}
	CRITICAL_SECTION lock; 
	ISocketBase* m_listener;
};

ref class DatagramMessage{
public:
	property DataReader^ Reader
	{		
        DataReader^ get()
		{
			return m_Reader;
		}
		void set(DataReader^ value)
		{
			m_Reader = value;
		}
	}
	
	property String^ RemotePort
	{
		void set(String^ value)
		{
			m_RemotePort = value;
		}
        String^ get()
		{
			return m_RemotePort;
		}
	}
	property String^ RemoteAddr
	{
		void set(String^ value)
		{
			m_RemoteAddr = value;
		}
        String^ get()
		{
			return m_RemoteAddr;
		}
	}
	property String^ LocalAddr
	{
		void set(String^ value)
		{
			m_LocalAddr = value;
		}
        String^ get()
		{
			return m_LocalAddr;
		}
	}
private:
	DataReader^						m_Reader;
	String^							m_RemotePort;
	String^							m_RemoteAddr;
	String^							m_LocalAddr;
};

ref class StreamingBuffer sealed
{
public:
	StreamingBuffer(Array<unsigned char>^ buff)
	{
		m_Buffer = buff;
		m_Size = buff->Length;
		m_CurrentPos = 0;
	}

	int Read(unsigned char* out, int len)
	{
		int readsize = ReadNoSeek(out, len);
		if(readsize > 0)
			m_CurrentPos += readsize;
		return readsize;
	}

	int ReadNoSeek(unsigned char* out, int len)
	{
		int unread = m_Size - m_CurrentPos;
		int readsize = (unread<len?unread:len);
		if(readsize < 0)
			readsize = 0;

		if (readsize > 0)
		{
			memcpy(out, m_Buffer->Data+m_CurrentPos, readsize);
		}
		return readsize;
	}

	void Seek(int len)
	{
		m_CurrentPos += len;
		if (m_CurrentPos > m_Size)
			m_CurrentPos = m_Size;
	}

	int GetUnConsumedSize()
	{
		int unread = m_Size - m_CurrentPos;
		if(unread < 0)
			unread = 0;
		return unread;
	}

private:
	WriteOnlyArray<unsigned char>^	m_Buffer;
	int						m_Size;
	int						m_CurrentPos;
};

class ISocketBase
{
public:
	ISocketBase(void);
	virtual ~ISocketBase(void);
	virtual int bind(string addr, int port) = 0;
	virtual int listen(int backlog) = 0;
	virtual int accept() = 0;
	virtual int connect(string dstaddr, int dstport) = 0;

	virtual int send(const char *buf, int len) = 0;
	virtual int sendto(const char *buf, int len, string dstaddr, int port) = 0;
	virtual int wsasendto(LPWSABUF lpBuffers, DWORD dwBufferCount, string dstaddr, int port) = 0;

	virtual int recv(char *buf, int len, int flags) = 0;
	virtual int recvfrom(char *buf, int len, int flags, string &fromIp, int &fromPort) = 0;
	virtual int wsarecvfrom(LPWSABUF lpBuffers, DWORD dwBufferCount, string &dstaddr, int &port) = 0;

	virtual int closesocket(bool needcancel = true) = 0;
	virtual string getlocaladdr() = 0;
	virtual int getlocalport() = 0;
	virtual string getremoteaddr() = 0;
	virtual int getremoteport() = 0;
	virtual int setsockopt(int level, int optname, const char* optval, int optlen) = 0;
	virtual int getsockopt(int level, int optname, char* optval, int *optlen) = 0;
	inline void SetAsyncState(AsyncState state, bool set=true);
	inline bool IsDoingAsync(AsyncState state)
	{
		return ((m_AsyncState&state) == state);
	}
	inline bool IsIdle(){
		return (m_AsyncState == IDLE);
	}
	inline void SetAsyncEvent(NonBlockEvent state, bool set=true){
		lock_guard<mutex> lock(m_AsyncEventMtx);
		if (set)
			m_NonBlockEvent = (m_NonBlockEvent | state);
		else
			m_NonBlockEvent = (m_NonBlockEvent & (~state));
	}	
	inline bool HasAsyncEvent(NonBlockEvent state)
	{
		return ((m_NonBlockEvent&state) == state);
	}
	inline int GetEvent()
	{
		return m_NonBlockEvent;
	}
	inline bool IsConnected()
	{
		return (m_Writer != nullptr && m_Reader != nullptr);
	}
	inline void SetRecvBlockSize(int size)
	{
		m_RecvBlockSize = size;
	}
	virtual void AddQueueConnection(StreamSocket^ client) = 0;
	virtual void AddQueueUDPMessage(DatagramSocketMessageReceivedEventArgs^ clientEvent) = 0;

protected:
	DataWriter^					m_Writer;
	DataReader^					m_Reader;
	cancellation_token_source	m_connecting_cts;
	cancellation_token_source	m_reading_cts;
	cancellation_token_source	m_writing_cts;
	bool						m_IsNonBlocking;
	int							m_AsyncState;
	int							m_NonBlockEvent;
	int							m_RecvBlockSize;
	WRTSockType					m_SocketType;
	ListenerContext^			m_listenerCtx;
	mutex						m_AsyncStateMtx;
	mutex						m_AsyncEventMtx;
};

class TCPSocket :
	public ISocketBase
{
public:
	TCPSocket(void);
	TCPSocket(StreamSocket^ client);
	~TCPSocket(void);
	virtual int bind(string addr, int port)
	{
		m_BindAddr = addr;
		m_BindPort = port;
		return 0;
	}
	virtual int listen(int backlog);
	virtual int accept();
	virtual int connect(string dstaddr, int dstport);

	virtual int send(const char *buf, int len);
	virtual int sendto(const char *buf, int len, string dstaddr, int port){
		return send(buf, len);
	}
	virtual int wsasendto(LPWSABUF lpBuffers, DWORD dwBufferCount, string dstaddr, int port);

	virtual int recv(char *buf, int len, int flags);
	virtual int recvfrom(char *buf, int len, int flags, string &fromIp, int &fromPort);
	virtual int wsarecvfrom(LPWSABUF lpBuffers, DWORD dwBufferCount, string &dstaddr, int &port);

	virtual int closesocket(bool needcancel = true);
	virtual string getlocaladdr();
	virtual int getlocalport();
	virtual string getremoteaddr();
	virtual int getremoteport();
	virtual int setsockopt(int level, int optname, const char* optval, int optlen);
	virtual int getsockopt(int level, int optname, char* optval, int *optlen);
	virtual void AddQueueConnection(StreamSocket^ client);
	virtual void AddQueueUDPMessage(DatagramSocketMessageReceivedEventArgs^ clientEvent);
	void LoadBufferLoop();
	void UpLoadBufferLoop();

private:
	StreamSocket^				m_Socket;
	bool						m_NoDealy;
	bool						m_KeepAlive;

	std::vector<StreamingBuffer^>	m_RecvBuffers;
	int							m_RecvBuffersTotalSize;
	mutex							m_RecvBuffMtx;

	std::vector<StreamingBuffer^>	m_SendBuffers;
	int							m_SendBuffersTotalSize;
	mutex							m_SendBuffMtx;
	
	//these are used to listening
	StreamSocketListener^		m_SocketListener;
	string						m_BindAddr;
	int							m_BindPort;
	int							m_MaxConn;
	std::vector<StreamSocket^>	m_ClientQueue;
};

class UDPSocket :
	public ISocketBase
{
public:
	UDPSocket(void);
	~UDPSocket(void);
	virtual int bind(string addr, int port);
	virtual int listen(int backlog);
	virtual int accept();
	virtual int connect(string dstaddr, int dstport);

	virtual int send(const char *buf, int len);
	virtual int sendto(const char *buf, int len, string dstaddr, int port);
	virtual int wsasendto(LPWSABUF lpBuffers, DWORD dwBufferCount, string dstaddr, int port);

	virtual int recv(char *buf, int len, int flags);
	virtual int recvfrom(char *buf, int len, int flags, string &fromIp, int &fromPort);
	virtual int wsarecvfrom(LPWSABUF lpBuffers, DWORD dwBufferCount, string &dstaddr, int &port);

	virtual int closesocket(bool needcancel);
	virtual string getlocaladdr();
	virtual int getlocalport();
	virtual string getremoteaddr();
	virtual int getremoteport();
	virtual int setsockopt(int level, int optname, const char* optval, int optlen);
	virtual int getsockopt(int level, int optname, char* optval, int *optlen);
	virtual void AddQueueConnection(StreamSocket^ client);
	virtual void AddQueueUDPMessage(DatagramSocketMessageReceivedEventArgs^ clientEvent);

private:
	DatagramSocket^					m_Socket;
	string							m_ConnectedAddr;
	int								m_ConnectedPort;
	mutex							m_MsgMtx;
	unsigned long long				m_UnConsumedBufferLen;
	std::vector<DatagramMessage^>	m_MsgQueue;
};