// DDServer.h: interface for the DDServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDSERVER_H__773399B6_1723_432C_84EE_F318762E5FD8__INCLUDED_)
#define AFX_DDSERVER_H__773399B6_1723_432C_84EE_F318762E5FD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable: 4786)
#include "Server.h"
#include "Player.h"
#include "../packets.h"
#include <map>
#include "Board.h"

#include "../ws-util.h"
#include "../Sconsole.h"

class DDServer : public Server, public Sconsole
{
public:
	DDServer(int port);
	virtual ~DDServer();
	virtual void tick(int ms);
	
	void newGame(int argc, char** argv);
	void kickPlayer(int argc, char** argv);
	void pause(int argc, char** argv);
	void unpause(int argc, char** argv);
	bool isPaused();

protected:
	virtual CLIENT_HANDLER_DIRECTIVE connect(CLIENT*);
	virtual void disconnect(CLIENT*);
	virtual void receive(CLIENT*);
	

private:
	std::map<int, Player*> players;
	Board* board;
	int playernum;
	int tts;
	bool paused;

};

#endif // !defined(AFX_DDSERVER_H__773399B6_1723_432C_84EE_F318762E5FD8__INCLUDED_)
