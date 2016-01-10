// Client.h: interface for the Client class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENT_H__E896982F_FADA_4638_B62F_34A657460F22__INCLUDED_)
#define AFX_CLIENT_H__E896982F_FADA_4638_B62F_34A657460F22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../packet.h"
#ifdef WIN32
#include <winsock2.h>
#else
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


#include <string>

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

//Error types
typedef enum CLIENT_ERROR {CERR_SEND_OK, CERR_SEND_ERROR, CERR_BADPACKET};

class Client  
{
public:
	Client(const char*, int port);
	Client();
	virtual ~Client();

//Controll functions
public:
	void connect( const char*, int port );
	void disconnect();

//Functions you'll want to overwrite
	virtual void onDisconnect();
	virtual void receive( PACKET*, int );

//Send data:
public:
	CLIENT_ERROR send( PACKET* );
	
//process data:
public:
	BOOL process();

//private functions
private:
	BOOL read();
	BOOL write();


//private data members:
protected:
	std::string rhost;
	int rport;
	SOCKET sd;
	bool connected;

//incoming fun
private:
	int inRead;
	int inSize;
	char buf[PACKET_MAX_LEN]; 

//Private outgoing queue
private:
	OUT_MESG *outH, *outT;
	int qLen;

};

#endif // !defined(AFX_CLIENT_H__E896982F_FADA_4638_B62F_34A657460F22__INCLUDED_)
