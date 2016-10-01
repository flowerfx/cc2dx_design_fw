///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKNetwork.h
///
/// Description :	RKNetwork Module
///----------------------------------------------------------------------
#ifdef OS_IOS
#include "RK.h"
#include "RKNetwork.h"
#include "RKHeap.h"
#include "RKMacros.h"
#include "RKThread.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h> // bk001204

#include <fcntl.h>
#include <errno.h>

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

//#import <net/if_types.h>
#import <arpa/inet.h>         // for inet_ntoa()
#import <net/if_dl.h>         // for 'struct sockaddr_dl'

#include "CoreFoundation/CFBundle.h"
#include "CoreFoundation/CFStream.h"
namespace RKUtils
{
///----------------------------------------------------------------------
/// RKSocketInternal
///----------------------------------------------------------------------
struct RKSocketInternal
{
	int socket;
	RKSocketType  type;
	bool          bBlocking;
};

///----------------------------------------------------------------------
/// Network status is queried when the network module is initialized
///----------------------------------------------------------------------
static bool RKNetwork_bNetworkAvailable = false;

///----------------------------------------------------------------------
/// We create this 'dummy' stream to force the WWAN interface up
/// sockets does not bring up the iphones wide area network interface
///----------------------------------------------------------------------
static CFReadStreamRef	RKNetwork_refReadStream = 0;
static CFWriteStreamRef	RKNetwork_refWriteStream = 0;

static void* RKNetworkInitWWAN_Job(void* pArg)
{
	// Default to fail
	RKNetwork_bNetworkAvailable = false;
	
	// Bring up the WWAN if possible
	CFStringRef host = CFSTR("www.steelstarstudios.com");	
	CFStreamCreatePairWithSocketToHost(kCFAllocatorDefault, host, 80, &RKNetwork_refReadStream, &RKNetwork_refWriteStream);
	if (RKNetwork_refReadStream && RKNetwork_refWriteStream)
	{
		// Attempt to open the write stream
		if (true == CFWriteStreamOpen(RKNetwork_refWriteStream))
		{
			CFStreamStatus status = CFWriteStreamGetStatus(RKNetwork_refWriteStream);
			while (status != kCFStreamStatusOpen && status != kCFStreamStatusError)
			{
				// Probably attempting to open
				RKThread_Yield();
				status = CFWriteStreamGetStatus(RKNetwork_refWriteStream);
			}
			
			// By this point there is an error or the stream has been opened
			if (status == kCFStreamStatusOpen)
			{
				if (true == CFWriteStreamCanAcceptBytes(RKNetwork_refWriteStream))
				{
					// Once the stream can accept data write something. Phone home would be nice here
//					const char* buffer = "Boo";
//					CFIndex bytesWritten = CFWriteStreamWrite(RKNetwork_refWriteStream, (uint8*)buffer, strlen(buffer));
//					if (bytesWritten == strlen(buffer))
					{
						// Sent excellent
						RKNetwork_bNetworkAvailable = true;
					}
				}

				// Close the opened stream
				CFWriteStreamClose(RKNetwork_refWriteStream);
			}
		}
	}
	
	return 0;
}

///----------------------------------------------------------------------
/// RKNetwork_HasWAN
///----------------------------------------------------------------------
bool RKNetwork_HasWAN()
{
	return RKNetwork_bNetworkAvailable;
}

///----------------------------------------------------------------------
/// RKNetwork_InitModule
///----------------------------------------------------------------------
void RKNetwork_InitModule()
{
	// Do this on a thread as is could block the main thread
//	RKJob* pJob = RKThread_AddJob(RKNetworkInitWWAN_Job, 0);
//	if (!pJob)
//	{
//		RKASSERT(false, "Could not bring up WWAN");
//	}
}

///----------------------------------------------------------------------
/// RKNetwork_DeinitModule
///----------------------------------------------------------------------
void RKNetwork_DeinitModule()
{
	// Release these sockets
	if (RKNetwork_refReadStream)
  {
		CFRelease(RKNetwork_refReadStream);
  }
		
	if (RKNetwork_refWriteStream)
  {
		CFRelease(RKNetwork_refWriteStream);
  }
}

///----------------------------------------------------------------------
/// RKNetwork_CreateSocket
///----------------------------------------------------------------------
RKSocket* RKNetwork_CreateSocket(RKSocketType socketType, bool bBlocking)
{
	RKSocketInternal* pSocketInternal = (RKSocketInternal*)RKHeap_AllocAndZero(sizeof(RKSocketInternal));
	pSocketInternal->socket = -1; // INVALID_SOCKET;
	pSocketInternal->type = socketType;
	pSocketInternal->bBlocking = bBlocking;
	
	// Set our protocol
	if (socketType == RKSOCKET_TCP)
	{
		// Reliable
		pSocketInternal->socket = socket(AF_INET, SOCK_STREAM, 0);

		// Disable Nagle
		/*
		BOOL noDelayOpt = TRUE;
		int noDelayOptLen = sizeof(BOOL);

		setsockopt(pSocketInternal->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayOpt, sizeof(BOOL));
		getsockopt(pSocketInternal->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayOpt, &noDelayOptLen);

		RKASSERT(noDelayOpt == TRUE, "Error could not disable Nagle algorithm.");
		*/
	}
	else if (socketType == RKSOCKET_UDP)
	{
		// Unreliable
		pSocketInternal->socket = socket(AF_INET, SOCK_DGRAM, 0);
	}

	if (pSocketInternal->socket != -1)
	{
		// To block or not to block
		RKNetwork_SetBlocking((RKSocket*)pSocketInternal, bBlocking);

		return (RKSocket*)pSocketInternal;
	}

	// Could not create socket
	RKHeap_Free(pSocketInternal);

	return 0;
}

///----------------------------------------------------------------------
/// RKNetwork_DestroySocket
///----------------------------------------------------------------------
void RKNetwork_DestroySocket(RKSocket** ppSocket)
{
	RKSocketInternal* pSocketInternal = (RKSocketInternal*)*ppSocket;
	*ppSocket = NULL;
	
	if (pSocketInternal)
	{
		close(pSocketInternal->socket);
		RKHeap_Free(pSocketInternal);
		pSocketInternal = NULL;
	}
}

///----------------------------------------------------------------------
/// RKNetwork_BindSocket
///----------------------------------------------------------------------
bool RKNetwork_BindSocket(RKSocket* pSocket, unsigned short port)
{
	if (pSocket)
	{
		RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
		
		sockaddr_in sin;
		memset(&sin, 0x00, sizeof(sockaddr_in));
		
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = INADDR_ANY;
		sin.sin_port = htons(port);
		
		if (bind(pSocketInternal->socket, (sockaddr*)&sin, sizeof(sockaddr_in)) != -1)
    {
			return true;
    }
	}
	
	return false;
}

///----------------------------------------------------------------------
/// RKNetwork_Listen
///----------------------------------------------------------------------
bool RKNetwork_Listen(RKSocket* pSocket)
{
	RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
	int ec = listen(pSocketInternal->socket, SOMAXCONN);
	if (ec == -1)
  {
		return false;
  }
	
	return true;
}

///----------------------------------------------------------------------
/// RKNetwork_Accept
///----------------------------------------------------------------------
RKSocket* RKNetwork_Accept(RKSocket* pSocket)
{
	RKSocketInternal* pServer = (RKSocketInternal*)pSocket;
	RKSocketInternal* pClient = NULL;
  
  sockaddr_in sin;
  uint32 length = sizeof(sockaddr_in);
	
  //int	accept(int, struct sockaddr * __restrict, socklen_t * __restrict) __DARWIN_ALIAS_C(accept);
  int clientSocket = accept(pServer->socket, (sockaddr*)&sin, (socklen_t*)&length);
  if (clientSocket == -1)
  {
	  // error
	  clientSocket = clientSocket;
/*
 int error = WSAGetLastError();
    if (error == WSAEWOULDBLOCK)
    {
      // No connection available
      error = error;
    }
 */
  }
  else
  {
    pClient = (RKSocketInternal*)RKHeap_AllocAndZero(sizeof(RKSocketInternal));
    pClient->socket = clientSocket;
  }

  return (RKSocket*)pClient;
}

///----------------------------------------------------------------------
/// RKNetwork_Connect
///----------------------------------------------------------------------
bool RKNetwork_Connect(RKSocket* pSocket, const char* pHostName, unsigned short port)
{
	if (pSocket)
	{
		RKSocketInternal* pClient = (RKSocketInternal*)pSocket;
		
		hostent* pHost = gethostbyname(pHostName);
		if (pHost)
		{
			struct sockaddr_in serverAddr;
			memset(&serverAddr, 0x00, sizeof(sockaddr_in));
			
			bcopy((char*)pHost->h_addr, (char*)&serverAddr.sin_addr.s_addr, pHost->h_length);
			serverAddr.sin_port = htons(port);
			
			int ec = connect(pClient->socket,(sockaddr*)&serverAddr, sizeof(serverAddr));
			if (ec < 0)
			{
				if (ec == EWOULDBLOCK)
				{
					// would block nothing happens
					return false;
				}
				else
				{
					// real error
					ec = ec;
					return false;
				}
			}
			else
			{
				return true;
			}
		}
	}
	
	return false;
}

///----------------------------------------------------------------------
/// RKNetwork_Disconnect
///----------------------------------------------------------------------
bool RKNetwork_Disconnect(RKSocket* pSocket)
{
	if (pSocket)
	{
		RKSocketInternal* pClient = (RKSocketInternal*)pSocket;
		pClient = pClient;
	}
	
	return false;
}

///----------------------------------------------------------------------
/// RKNetwork_SendSecure
///----------------------------------------------------------------------
int32 RKNetwork_SendSecure(RKSocket* pSocket, void* pBuffer, int32 bufferSize, bool* bError)
{
	return RKNetwork_Send(pSocket, pBuffer, bufferSize);
}

///----------------------------------------------------------------------
/// RKNetwork_Send
///----------------------------------------------------------------------
int32 RKNetwork_Send(RKSocket* pSocket, void* pBuffer, int32 bufferSize, bool* bError)
{
	if (pSocket && bufferSize)
	{
		RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
		int bytesSent = send(pSocketInternal->socket, (const char*)pBuffer, bufferSize, 0);
		if (bytesSent == -1)
		{
			return 0;
		}
		
		return bytesSent;
	}
	
	return 0;
}

///----------------------------------------------------------------------
/// RKNetwork_Receive
///----------------------------------------------------------------------
int32 RKNetwork_Receive(RKSocket* pSocket, void* pBuffer, int32 bufferSize, bool* bError)
{
	if (pSocket && bufferSize)
	{
		RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
		int bytesReceived = recv(pSocketInternal->socket, (char*)pBuffer, bufferSize, 0);
		if (bytesReceived == -1)
		{
			// error handling
			return 0;
		}

		return bytesReceived;
	}

	return 0;
}

///----------------------------------------------------------------------
/// RKNetwork_CanReadWrite
///----------------------------------------------------------------------
void RKNetwork_CanReadWrite(RKSocket* pSocket, bool& bCanRead, bool& bCanWrite)
{
	RKASSERT(false, "TODO");
	
//	bCanRead = false;
//  bCanWrite = false;
//  
//  if (pSocket)
//  {
//    RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
//
//    fd_set readSet;
//    readSet.fd_array[0] = pSocketInternal->socket;
//    readSet.fd_count = 1;
//
//    fd_set writeSet;
//    writeSet.fd_array[0] = pSocketInternal->socket;
//    writeSet.fd_count = 1;
//
//    timeval timeOut;
//    timeOut.tv_sec = 0;
//    timeOut.tv_usec = 1;
//
//    int ec = select(0, &readSet, &writeSet, NULL, &timeOut);
//    if (ec == -1)
//    {
//		ec = ec;
//    }
//    else if (ec == 0)
//    {
//      // timed out
//    }
//    else 
//    {
//      if (readSet.fd_count == 1 && readSet.fd_array[0] == pSocketInternal->socket)
//      {
//        bCanRead = true;
//      }
//      
//      if (writeSet.fd_count == 1 && writeSet.fd_array[0] == pSocketInternal->socket)
//      {
//        bCanWrite = true;
//      }
//    }
//  }
}

///----------------------------------------------------------------------
/// RKNetwork_CanWrite
///----------------------------------------------------------------------
bool RKNetwork_CanWrite(RKSocket* pSocket)
{
	RKASSERT(false, "TODO");
	
/*
 PROFILE2();
  
  if (pSocket)
  {
    RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
    
    fd_set writeSet;
    writeSet.fd_array[0] = pSocketInternal->socket;
    writeSet.fd_count = 1;

    timeval timeOut;
    timeOut.tv_sec = 0;
    timeOut.tv_usec = 1;

    int ec = select(0, NULL, &writeSet, NULL, &timeOut);
    if (ec == SOCKET_ERROR)
    {
      ec = WSAGetLastError();
    }
    else if (ec == 0)
    {
      // timed out
    }
    else 
    {
      if (writeSet.fd_count == 1 && writeSet.fd_array[0] == pSocketInternal->socket)
      {
        return true;
      }
    }
  }
  
*/
	return false;
}

///----------------------------------------------------------------------
/// RKNetwork_CanRead
///----------------------------------------------------------------------
bool RKNetwork_CanRead(RKSocket* pSocket)
{
	RKASSERT(false, "TODO");
	
/*
 PROFILE2();
  
  if (pSocket)
  {
    RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;

    fd_set readSet;
    readSet.fd_array[0] = pSocketInternal->socket;
    readSet.fd_count = 1;

    timeval timeOut;
    timeOut.tv_sec = 0;
    timeOut.tv_usec = 1;

    int ec = select(0, &readSet, NULL, NULL, &timeOut);
    if (ec == SOCKET_ERROR)
    {
      ec = WSAGetLastError();
    }
    else if (ec == 0)
    {
      // timed out
    }
    else 
    {
      if (readSet.fd_count == 1 && readSet.fd_array[0] == pSocketInternal->socket)
      {
        return true;
      }
    }
  }

*/
 return false;
}

///----------------------------------------------------------------------
/// RKNetwork_DeinitModule
///----------------------------------------------------------------------
void RKNetwork_SetBlocking(RKSocket* pSocket, bool bBlocking)
{
  RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
	
	int arg = bBlocking ? 1 : 0;
	int err = ioctl(pSocketInternal->socket, FIONBIO, (char*)&arg); 
  if (err)
  {
	  RKASSERT(false, "Err");
  }
}
}
#endif