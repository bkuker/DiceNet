/***********************************************************************
 ws-util.h - Declarations for the Winsock utility functions module.
***********************************************************************/



// Uncomment one.
//#include <winsock.h>
#ifdef WIN32
#include <winsock2.h>
#else 
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET int
#define u_long in_addr_t
#define u_short short
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (~0)
#endif

#ifdef WIN32
extern const char* WSAGetLastErrorMessage(const char* pcMessagePrefix, int nErrorID = 0);
#endif

extern bool ShutdownConnection(SOCKET sd);

extern u_long LookupAddress(const char* pcHost);
extern SOCKET EstablishConnection(u_long nRemoteAddr, u_short nPort);

extern SOCKET SetUpListener(const char* pcAddress, int nPort);
