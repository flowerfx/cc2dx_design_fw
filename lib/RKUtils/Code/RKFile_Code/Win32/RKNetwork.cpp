///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	RKNetwork.h
///
/// Description :	RKNetwork Module
///----------------------------------------------------------------------

#include "RK.h"
#include "RKNetwork.h"
#include "RKHeap.h"
#include "RKMacros.h"
#include "RKThread.h"

#ifdef OS_W8
#include "metrosockdef.h"
#else
#include <winsock2.h>
#endif
#include <fcntl.h>
#include <errno.h>

///----------------------------------------------------------------------
/// RKSocketInternal
///----------------------------------------------------------------------
struct RKSocketInternal
{
  SOCKET        socket;
  RKSocketType  type;
  bool          bBlocking;
};

///----------------------------------------------------------------------
/// Network status is queried when the network module is initialized
///----------------------------------------------------------------------
static bool RKNetwork_bNetworkAvailable = false;

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
  WSADATA wsaData;

  WORD version = MAKEWORD(2,2);

  if (0 == WSAStartup(version, &wsaData))
  {
    RKNetwork_bNetworkAvailable = true;
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
      WSACleanup();
      RKNetwork_bNetworkAvailable = false;
    }
  }
}

///----------------------------------------------------------------------
/// RKNetwork_DeinitModule
///----------------------------------------------------------------------
void RKNetwork_DeinitModule()
{
  WSACleanup();
}

///----------------------------------------------------------------------
/// RKNetwork_CreateSocket
///----------------------------------------------------------------------
RKSocket* RKNetwork_CreateSocket(RKSocketType socketType, bool bBlocking)
{
  RKSocketInternal* pSocketInternal = (RKSocketInternal*)RKHeap_AllocAndZero(sizeof(RKSocketInternal));
  pSocketInternal->socket = INVALID_SOCKET;
  pSocketInternal->type = socketType;
  pSocketInternal->bBlocking = bBlocking;

  // Set our protocol
  if (socketType == RKSOCKET_TCP)
  {
    // Reliable
    pSocketInternal->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Disable Nagle
    BOOL noDelayOpt = TRUE;
    int noDelayOptLen = sizeof(BOOL);

    setsockopt(pSocketInternal->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayOpt, sizeof(BOOL));
    getsockopt(pSocketInternal->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelayOpt, &noDelayOptLen);

    RKASSERT(noDelayOpt == TRUE, "Error could not disable Nagle algorithm.");
  }
  else if (socketType == RKSOCKET_UDP)
  {
    // Unreliable
    pSocketInternal->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  }

  if (pSocketInternal->socket != INVALID_SOCKET)
  {
    unsigned long nonBlocking = 1;
    if (bBlocking)
    {
      // return the default blocking socket
      return (RKSocket*)pSocketInternal;
    }
    else if (ioctlsocket(pSocketInternal->socket, FIONBIO, &nonBlocking) == 0)
    {
      // return the non-blocking socket
      return (RKSocket*)pSocketInternal;
    }
  }

  // Could not create socket
  RKHeap_Free(pSocketInternal);
  return NULL;
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
    closesocket(pSocketInternal->socket);

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

    if (bind(pSocketInternal->socket, (SOCKADDR*)&sin, sizeof(sockaddr_in)) != SOCKET_ERROR)
      return true;
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
  if (ec == SOCKET_ERROR)
    return false;

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
  int length = sizeof(sockaddr_in);

  SOCKET clientSocket = accept(pServer->socket, (SOCKADDR*)&sin, &length);
  if (clientSocket == INVALID_SOCKET)
  {
    int error = WSAGetLastError();
    if (error == WSAEWOULDBLOCK)
    {
      // No connection available
      error = error;
    }
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

  sockaddr_in sin;
    memset(&sin, 0x00, sizeof(sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ((struct in_addr *)(pHost->h_addr))->s_addr;
    sin.sin_port = htons(port);

    int ec = connect(pClient->socket, (SOCKADDR*)&sin, sizeof sin);
    if (ec == SOCKET_ERROR)
    {
      ec = WSAGetLastError();
      if (ec == WSAEWOULDBLOCK || ec == WSAEALREADY || ec == WSAEINVAL)
  {
        // Use the select function to determine the completion of the connection request by checking to see if the socket is writable.
        return RKNetwork_CanWrite(pSocket);
      }
      else if (ec == WSAEISCONN)
    {
        // connection completed
        return true;
    }
  }
  else
  {
    return true;
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
  }

  return false;
}

///----------------------------------------------------------------------
/// RKNetwork_SendSecure
///----------------------------------------------------------------------
int32 RKNetwork_SendSecure(RKSocket* pSocket, void* pBuffer, int32 bufferSize, bool* bError)
{
  return RKNetwork_Send(pSocket, pBuffer, bufferSize, bError);
}

///----------------------------------------------------------------------
/// RKNetwork_Send
///----------------------------------------------------------------------
int32 RKNetwork_Send(RKSocket* pSocket, void* pBuffer, int32 bufferSize, bool* bError)
{
  if (bError)
    *bError = false;

  if (pSocket && bufferSize)
  {
    RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
    int bytesSent = send(pSocketInternal->socket, (const char*)pBuffer, bufferSize, 0);
    if (bytesSent == SOCKET_ERROR)
    {
      bytesSent = WSAGetLastError();
      //RKWarningOut("Winsock Error #%d", bytesSent);

      if (bError)
        *bError = true;

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
  if (bError)
    *bError = false;

  if (pSocket && bufferSize)
  {
    RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
    int bytesReceived = recv(pSocketInternal->socket, (char*)pBuffer, bufferSize, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
      bytesReceived = WSAGetLastError();
      //RKWarningOut("Winsock Error #%d", bytesReceived);

      if (bError)
        *bError = true;

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
  bCanRead = false;
  bCanWrite = false;

  if (pSocket)
  {
    RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;

    fd_set readSet;
    readSet.fd_array[0] = pSocketInternal->socket;
    readSet.fd_count = 1;

    fd_set writeSet;
    writeSet.fd_array[0] = pSocketInternal->socket;
    writeSet.fd_count = 1;

    timeval timeOut;
    timeOut.tv_sec = 0;
    timeOut.tv_usec = 1;

    int ec = select(0, &readSet, &writeSet, NULL, &timeOut);
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
        bCanRead = true;
      }

      if (writeSet.fd_count == 1 && writeSet.fd_array[0] == pSocketInternal->socket)
      {
        bCanWrite = true;
      }
    }
  }
}

///----------------------------------------------------------------------
/// RKNetwork_CanWrite
///----------------------------------------------------------------------
bool RKNetwork_CanWrite(RKSocket* pSocket)
{
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

  return false;
}

///----------------------------------------------------------------------
/// RKNetwork_CanRead
///----------------------------------------------------------------------
bool RKNetwork_CanRead(RKSocket* pSocket)
{
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

  return false;
}

///----------------------------------------------------------------------
/// RKNetwork_DeinitModule
///----------------------------------------------------------------------
void RKNetwork_SetBlocking(RKSocket* pSocket, bool bBlocking)
{
  RKASSERT(false, "TODO");
//   RKSocketInternal* pSocketInternal = (RKSocketInternal*)pSocket;
// 
//   int arg = bBlocking ? 1 : 0;
//   int err = ioctl(pSocketInternal->socket, FIONBIO, (char*)&arg); 
//   if (err)
//   {
//     RKASSERT(false, "Err");
//   }
}
