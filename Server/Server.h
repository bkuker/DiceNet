// Server.h: interface for the Server class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVER_H__80CD9546_06DF_41D6_BBF8_9F24345C2E6B__INCLUDED_)
#define AFX_SERVER_H__80CD9546_06DF_41D6_BBF8_9F24345C2E6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../packet.h"
#include <vector>
#include <iostream>
#ifdef WIN32
#include <winsock2.h>
#else
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
typedef int SOCKET;
typedef bool BOOL;
#define TRUE true
#define FALSE false
/* #define u_long in_addr_t; */
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (~0)
#endif

//Client structs
#ifndef __Q__
#define __Q__
struct OUT_MESG{
	int size;
	char* data;
	OUT_MESG* next;
	OUT_MESG() :
		size(0), data(NULL), next(NULL)
	{};
	~OUT_MESG(){
		if (data != NULL)
			delete data;
	};
	void free(){
		if (data != NULL){
			delete data;
			data = NULL;
		}
		if (next != NULL){
			next->free();
			delete next;
		}
	};
};
#endif

struct CLIENT{
    SOCKET sd;
    char inBuffer[PACKET_MAX_LEN];
    int inSize;
	int inRead;
	void* userData;
	OUT_MESG *outH, *outT;
	int qLen;

	CLIENT(SOCKET sd_) : 
		sd(sd_), inSize(0), inRead(0), qLen(0),
		userData(NULL), outH(NULL), outT(NULL) 
	{};

	~CLIENT(){
		if (outH != NULL){
			outH->free();
			delete outH;
		}
	};
};


#define ALL_CLIENTS (CLIENT*)-1

typedef enum CLIENT_HANDLER_DIRECTIVE {ACCEPT, DENY};


//Error types
typedef enum SERVER_ERROR {SERR_SEND_OK};

class Server  
{
public:
	//Server(int port);
	Server(const char* addr, int port);
	Server(int port);
	virtual ~Server();

//Handlers you'll want to replace:
protected:
	virtual CLIENT_HANDLER_DIRECTIVE connect(CLIENT*);
	virtual void disconnect(CLIENT*);
	virtual void receive(CLIENT*);

public:
	void send( CLIENT*, PACKET* );
	void sendAll( PACKET* );
	BOOL process();






//private functions
protected:
	BOOL read( CLIENT* );
	BOOL write( CLIENT* );
#ifdef WIN32
	void SetupFDSets(fd_set& ReadFDs, fd_set& WriteFDs, fd_set& ExceptFDs);
#else
	void SetupFDSets(fd_set& ReadFDs, fd_set& WriteFDs, fd_set& ExceptFDs, int* max);
#endif
	void debug( std::string );

//private data members:
protected:
	std::vector<CLIENT*> clients;
	SOCKET listener;
};

#endif // !defined(AFX_SERVER_H__80CD9546_06DF_41D6_BBF8_9F24345C2E6B__INCLUDED_)
