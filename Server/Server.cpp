// Server.cpp: implementation of the Server class.
//
//////////////////////////////////////////////////////////////////////

#include "Server.h"
#include "../ws-util.h"

#ifndef WIN32
#include <stdio.h>
#endif

#define MAX_QLEN 200

void pprint(PACKET* p){
	printf("Size %02x\n", p->size);
	for (int i = 0; i < p->size; i++)
		printf("%02x\n", (byte)((byte*)p)[i]);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Server::Server(const char* addr, int port)
{	
#ifdef WIN32
    WSAData wsaData;
	int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
		std::cerr << "WSAStartup() returned error code " << nCode << "." <<
				std::endl;
	}
#endif
	listener =  SetUpListener(addr, port);
	if ( listener == INVALID_SOCKET )
		throw "Couldn't listen!";
	std::cout << "Listening at " << addr << " on " << port << std::endl;
}

Server::Server(int port)
{	
#ifdef WIN32
    WSAData wsaData;
	int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
		std::cerr << "WSAStartup() returned error code " << nCode << "." <<
				std::endl;
	}
#endif
	listener =  SetUpListener(NULL, port);
        if ( listener == INVALID_SOCKET )
                throw "Couldn't listen!";
	std::cout << "Listening on " << port << std::endl;
}

Server::~Server()
{

}

//////////////////////////////////////////////////////////////////////
// Handlers to overwrite
//////////////////////////////////////////////////////////////////////
CLIENT_HANDLER_DIRECTIVE Server::connect(CLIENT* c)
{
	static int count = 0;
	debug("Accepted connection");
	c->userData = (char*)count;
	count++;
	return ACCEPT;
}

void Server::disconnect(CLIENT* c)
{
	std::cout << "Client " << (int)c->userData << " disconnected" << std::endl;
}

void Server::receive(CLIENT* c)
{
	//std::cout	<< "Server received packet sized " << c->inSize
	//			<< " from client " << (int)c->userData << std::endl;
	printf("REC-------------------------------\n");
	pprint((PACKET*)c->inBuffer);
	send(c, (PACKET*)c->inBuffer );
}

//////////////////////////////////////////////////////////////////////
// Public functions
//////////////////////////////////////////////////////////////////////
void Server::send( CLIENT* c, PACKET* b )
{
	printf("SND-------------------------------\n");
	pprint(b);
	if ( c == ALL_CLIENTS ){
		sendAll( b );
		return;
	}

	int l = ((PACKET*)b)->size;
	if (l > PACKET_MAX_LEN)
		l = PACKET_MAX_LEN;

	//If there is no queue start one
	if ( c->outH == NULL ){
		c->outH = new OUT_MESG;
		c->outT = c->outH;
	} else { //else add to the tail
		c->outT->next = new OUT_MESG;
		c->outT = c->outT->next;
	}
	c->qLen++;
	c->outT->data = new char[l];
	memcpy(c->outT->data, b, l);
	c->outT->size = l;
}

void Server::sendAll( PACKET* b )
{
	for (	std::vector<CLIENT*>::iterator it = clients.begin();
	it != clients.end(); it++)
	{
		//send the chat back out
		send( *it, b );
	}
}


BOOL Server::process(){
	BOOL ret = false;
    sockaddr_in sinRemote;
    int nAddrSize = sizeof(sinRemote);
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 20;
	fd_set ReadFDs, WriteFDs, ExceptFDs;
#ifdef WIN32
    	SetupFDSets(ReadFDs, WriteFDs, ExceptFDs);
	if (select(0, &ReadFDs, &WriteFDs, &ExceptFDs, &tv) > 0) {
#else
	int maxFD;
	SetupFDSets(ReadFDs, WriteFDs, ExceptFDs, &maxFD);
	if (select(maxFD+1, &ReadFDs, &WriteFDs, &ExceptFDs, &tv) > 0) {
#endif	
		ret = true;
		//// Something happened on one of the sockets.
		// Was it the listener socket?...
		if (FD_ISSET(listener, &ReadFDs)) {
			SOCKET sd = accept(listener, (sockaddr*)&sinRemote, &nAddrSize);
			if (sd != INVALID_SOCKET) {
				
				CLIENT* c = new CLIENT(sd);

				//NEW CONNECTION
				if ( connect( c ) == ACCEPT )
					clients.push_back(c);
				else
					delete c;
				

#ifdef WIN32
				// Mark the socket as non-blocking, for safety.
				u_long nNoBlock = 1;
				ioctlsocket(sd, FIONBIO, &nNoBlock);
#endif
			}
			else {
#ifdef WIN32
				debug( WSAGetLastErrorMessage("accept() failed") );
#else
				debug( "accept() failed" );
#endif
				return false;
			}
		}
		else if (FD_ISSET(listener, &ExceptFDs)) {
			int err;
			int errlen = sizeof(err);
			getsockopt(listener, SOL_SOCKET, SO_ERROR,
				(char*)&err, &errlen);
#ifdef WIN32
				debug( WSAGetLastErrorMessage("Exception on listening socket: ", err) );
#else
				debug( "Exception on listening socket." );
#endif
			return false;
		}
		
		// ...Or was it one of the client sockets?
		std::vector<CLIENT*>::iterator it = clients.begin();
		while (it != clients.end()) {
			BOOL bOK = TRUE;
			const char* pcErrorType = 0;
			
			// See if this socket's flag is set in any of the FD
			// sets.
			if (FD_ISSET((*it)->sd, &ExceptFDs)) {
				bOK = false;
				pcErrorType = "General socket error";
				FD_CLR((*it)->sd, &ExceptFDs);
			}
			else {
				if (FD_ISSET((*it)->sd, &ReadFDs)) {
					//debug( "Socket became readable" );
					pcErrorType = "Read error";

					///READ DATA
					bOK = read(*it);

					FD_CLR((*it)->sd, &ReadFDs);
				}
				if (FD_ISSET((*it)->sd, &WriteFDs)) {
					//debug( "Socket became writable" );					
					pcErrorType = "Write error";
					
					///WRITE DATA
					bOK = write(*it);

					FD_CLR((*it)->sd, &WriteFDs);
				}
			}
			
			if (!bOK) {
				// Something bad happened on the socket, or the
				// client closed its half of the connection.  Shut
				// the conn down and remove it from the list.
#ifdef WIN32
//I think this is broken anyway, casting an int* to a char*? 
//I didnt write it anyway :-D
				int err;
				int errlen = sizeof(err);
				getsockopt((*it)->sd, SOL_SOCKET, SO_ERROR, (char*)&err, &errlen);
				if (err != NO_ERROR) {
					debug (WSAGetLastErrorMessage(pcErrorType, err) );
				}
#endif
				///LOST CLIENT
				disconnect( *it );
				ShutdownConnection((*it)->sd);
				delete *it;
				clients.erase(it);
				it = clients.begin();
			}
			else {
				// Go on to next connection
				++it;
			}
		}
	}
/*	else {
#ifdef WIN32
		debug( WSAGetLastErrorMessage("select() failed") );
#else
		debug( "select() failed" );
#endif
	}*/
	
	//Run through clients and if any have to big a send
	//queue kill'em
	std::vector<CLIENT*>::iterator it = clients.begin();
	while (it != clients.end()) {
		if ( (*it)->qLen >= MAX_QLEN ){
			debug("Timed out on client waiting to send");
			///LOST CLIENT
			disconnect( *it );
			
			ShutdownConnection((*it)->sd);
			clients.erase(it);
			it = clients.begin();
			
		}
		++it;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////
// Private Fcns
//////////////////////////////////////////////////////////////////////////////////
BOOL Server::read( CLIENT* c){
	int ins;
	if ( c->inRead < sizeof(PACKET) ){
			c->inRead += ins = recv( c->sd, c->inBuffer, sizeof(PACKET), 0);
			if ( c->inRead == sizeof(PACKET) )
				c->inSize = ((PACKET*)c->inBuffer)->size;
	} else if ( c->inRead < c->inSize ){
		c->inRead += ins = recv( c->sd, c->inBuffer + c->inRead, c->inSize - c->inRead, 0);
	}

	if ( c->inRead == c->inSize && c->inSize != 0 ){
		receive( c );
		c->inSize = 0;
		c->inRead = 0;
	}
	return ins != -1 && ins != 0;
}

BOOL Server::write( CLIENT* c){
	if ( c->outH != NULL ){
		::send( c->sd, c->outH->data, c->outH->size, 0);
		OUT_MESG* t = c->outH;
		c->outH = c->outH->next;
		delete t;
		c->qLen--;
		std::cout << "Len: " << c->qLen << std::endl;
	}

	return TRUE;
}



#ifdef WIN32
void Server::SetupFDSets(fd_set& ReadFDs, fd_set& WriteFDs, fd_set& ExceptFDs) 
#else
void Server::SetupFDSets(fd_set& ReadFDs, fd_set& WriteFDs, fd_set& ExceptFDs, int* max)
#endif
{
    FD_ZERO(&ReadFDs);
    FD_ZERO(&WriteFDs);
    FD_ZERO(&ExceptFDs);
#ifndef WIN32
    *max = 0;
#endif
    // Add the listener socket to the read and except FD sets, if there
    // is one.
    if (listener != INVALID_SOCKET) {
        FD_SET(listener, &ReadFDs);
        FD_SET(listener, &ExceptFDs);
#ifndef WIN32
    	*max = listener;
#endif
    } else {
	debug("can't add listener to fsets");
    }

    // Add client connections
    std::vector<CLIENT*>::iterator it = clients.begin();
    while (it != clients.end()) {
	//always pay attention to incoming data
	FD_SET((*it)->sd, &ReadFDs);
#ifndef WIN32
	if ( (*it)->sd > *max ) *max = (*it)->sd;
#endif
        if ((*it)->outH != NULL) {
            // There's data still to be sent on this socket, so we need
            // to be signalled when it becomes writable.
            FD_SET((*it)->sd, &WriteFDs);
        }
        FD_SET((*it)->sd, &ExceptFDs);
        ++it;
    }
}

void Server::debug( std::string s ){
	std::cout << s.c_str() << std::endl;
}
