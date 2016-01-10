// Client.cpp: implementation of the Client class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include "stdafx.h"
#include <winsock2.h>
#else
#define INADDR_NONE 0xffffffff
#endif

#include "Client.h"
#include "../ws-util.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Client::Client(const char* host, int port)
{
	rhost = "";
	rport = 0;
	outH = outT	= NULL;
	qLen = 0;
	inRead = 0;
	inSize = 0;
	connected = false;
	connect(host, port);
}

Client::Client()
{
	rhost = "";
	rport = 0;
	outH = outT	= NULL;
	qLen = 0;
	inRead = 0;
	inSize = 0;
	connected = false;
}

Client::~Client()
{
	disconnect();
	if (outH != NULL){
		outH->free();
		delete outH;
	}
}

//Controll functions
void Client::connect( const char* host, int port ){
	if (connected)
		throw "Already connected.";

	//Store the host and port
	rport = port;
	rhost = host;

#ifdef WIN32
    // Start Winsock up
    WSAData wsaData;
	int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
		throw "WSAStartup() returned error code.";
    }
#endif

	//Look up address and throw an error if there is a failure
	u_long nRemoteAddress = LookupAddress(rhost.c_str());
    if (nRemoteAddress == INADDR_NONE) {
#ifdef WIN32
		throw WSAGetLastErrorMessage("Lookup address");
#else
		throw "Unable to lookup address.";
#endif

    }
	
	
    // Connect to the server
    sd = EstablishConnection(nRemoteAddress, htons(rport));
    if (sd == INVALID_SOCKET) {
#ifdef WIN32
        throw WSAGetLastErrorMessage("Connect to server");
#else
	throw "Unable to connect to server";
#endif
    }
	connected = true;
}

void Client::disconnect(){
	ShutdownConnection(sd);
	inRead = inSize = 0;
	if (outH != NULL){
		outH->free();
		delete outH;
	}
	outH = outT	= NULL;
	qLen = 0;
	onDisconnect();
	connected = false;
}

void Client::onDisconnect()
{
	std::cout << "Client got disconnected" << std::endl;
}

void Client::receive( PACKET* b, int l )
{
	std::cout << "Client received length " << l << std::endl;
}

//Send data:
CLIENT_ERROR Client::send( PACKET* b ){
	//If there is no queue start one
	if ( outH == NULL ){
		outH = new OUT_MESG;
		outT = outH;
	} else { //else add to the tail
		outT->next = new OUT_MESG;
		outT = outT->next;
	}
	qLen++;
	outT->data = new char[b->size];
	memcpy(outT->data, b, b->size);
	outT->size = b->size;
	return CERR_SEND_OK;
}

//private functions
BOOL Client::process(){
	if (!connected)
		return TRUE;
	fd_set ReadFDs, WriteFDs;
    FD_ZERO(&ReadFDs);
    FD_ZERO(&WriteFDs);
	FD_SET(sd, &ReadFDs);
	FD_SET(sd, &WriteFDs);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 300;

	if (select(0, &ReadFDs, &WriteFDs, NULL, &tv) > 0){
		if (FD_ISSET(sd, &ReadFDs)){
			if (!read()){
				disconnect();
			}
		}
		if (FD_ISSET(sd, &WriteFDs))
			write();
	}
	return TRUE;
}

BOOL Client::read(){
	int ins;
	if ( inRead < sizeof(PACKET) ){
			inRead += ins = recv( sd, buf, sizeof(PACKET), 0);
			if ( inRead == sizeof(PACKET) )
				inSize = ((PACKET*)buf)->size;
	} else if ( inRead < inSize ){
		inRead += ins = recv(sd, buf + inRead, inSize - inRead, 0);
	}

	if ( inRead == inSize && inSize != 0 ){
		receive( (PACKET*)buf, inSize );
		inSize = 0;
		inRead = 0;
	}
	return ins != -1 && ins != 0;
}

BOOL Client::write(){
	if ( outH != FALSE ){
		::send( sd, outH->data, outH->size, 0);
		OUT_MESG* t = outH;
		outH = outH->next;
		delete t;
		qLen--;
	}
	return TRUE;
}