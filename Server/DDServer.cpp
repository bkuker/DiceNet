// DDServer.cpp: implementation of the DDServer class.
//
//////////////////////////////////////////////////////////////////////

#include "DDServer.h"
#include <iostream>
#ifdef WIN32
#include <stdlib.h> //for sleep()
#else
#include <unistd.h>
#include<sys/timeb.h>
#endif
#define safeDelete(x) if ( x != NULL ){ delete x; x = NULL; }

////////////////////////
// callbacks
////////////////////////

// setups up a new game
// @new
void DDServer::newGame(int argc, char** argv){
	
	// there should only be one argument - the dimensions of the 
	// board that we're starting.
	int size;
	
	if( argc > 1 ){ // just to be safe
		size = atoi(argv[1]);
	} else {
		size = 8; // default value
	}

	//get ready to start a game...
	safeDelete(board);
	board = new Board(this, size);
	board->tellClient(ALL_CLIENTS);
	std::map<int, Player*>::iterator it = players.begin();
	
	while (it != players.end() ){
		Player* p = it->second;
		
		//Slap them in somewhere.
		p->setPosition(board->cell(0,0), NONE);
		board->cell(0,0)->addPlayer(p);
		p->tellClient( ALL_CLIENTS );
		it++;

	}
	

} // end of newGame method

// these two methods are CS1 methods.  Sorry :-(
void DDServer::pause(int argc, char** argv){
	paused = true;
}

void DDServer::unpause(int argc, char** argv){
	paused = false;
}

// this method kicks player(s).  Pass the string name(s) of the 
// players that you want to kick and it will disconnect 
// them from the server 
void DDServer::kickPlayer(int argc, char** argv){

	std::map<int, Player*>::iterator it;
	

	// loop through all of the players that were passed in
	// we start at 1 because 0 is the name of the callback or 
	// alias
	for(int j=1; j < argc; j++){

		// kick current player in array 
		// This means we get a pointer to the client.
		it = players.begin();

		// loop through all players to find a name match
		while (it != players.end() ){
			
			Player* p = it->second; // because of map/iterator stuff 

			// string comparison
			if( !strcmp( p->getName().data(), argv[j]) ){
				// ba bye!
				CLIENT *temp = (CLIENT *)p->cli_struct;

				ShutdownConnection(temp->sd);
			} // end of if
			it++; // next player
		} // end of while loop 
	} // end of for loop 
} // end of kickPlayer


// @end new

// another CS1 method :-(
bool DDServer::isPaused(){
	return paused;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DDServer::DDServer(int port):Server(port), Sconsole()
{
	paused = false;

	playernum = 1;
	board = new Board(this, 8);//NULL;
	
	// setup commands for the server 
	registerConsoleCommand("board", (CONSOLE_CALLBACK_OBJECT)this, (CONSOLE_CALLBACK_FUNCTION2)((&DDServer::newGame)) );
	registerConsoleCommand("kickPlayer", (CONSOLE_CALLBACK_OBJECT)this, (CONSOLE_CALLBACK_FUNCTION2)((&DDServer::kickPlayer)) );
	registerConsoleCommand("pause", (CONSOLE_CALLBACK_OBJECT)this, (CONSOLE_CALLBACK_FUNCTION2)((&DDServer::pause)) );
	registerConsoleCommand("unpause", (CONSOLE_CALLBACK_OBJECT)this, (CONSOLE_CALLBACK_FUNCTION2)((&DDServer::unpause)) );

}

DDServer::~DDServer()
{

}

void DDServer::tick(int ms){
	BOOL d = true;
	for (int i = 0; i < 30 && d; i++)
		d = process();

	if ( players.empty() ){
		//Sleep for a second then return
#ifdef WIN32
		_sleep(1000);
#else
		sleep(1);
#endif
		return;
	} else if ( ms > 999 ) {
		std::cout << "Waking up..." << std::endl;
		return;
	}

	if ( ms > 100 )
		std::cout << "Long server frame!" << std::endl;

	if (board != NULL){
		std::map<int, Player*>::iterator it = players.begin();
		while (it != players.end() ){
			(it->second)->tick(ms);
			it++;
		}
		board->tick(ms);
		if(board->gameOver()){
			safeDelete(board);
			Player::totalScore = 0;
			it = players.begin();
			while (it != players.end() ){
				Player* p = it->second;
				send(ALL_CLIENTS, (PACKET*)&P_PLAYERSTAT(p->playernum, p->stats) );
				p->reset();
				it++;
			}
			send(ALL_CLIENTS, (PACKET*)&P_MSG(MSG_GAMEOVER));
			tts=10;
			send(ALL_CLIENTS, (PACKET*)&P_MSG(MSG_TTS, tts));
		}
	} else {
		static int mss = 0;
		mss += ms;
		if (mss > 1000){
			tts--;
			mss-=1000;
			send(ALL_CLIENTS, (PACKET*)&P_MSG(MSG_TTS, tts));
		}
		if (tts == 0){
			//get ready to start a game...
			safeDelete(board);
			board = new Board(this, 8);
			board->tellClient(ALL_CLIENTS);
			std::map<int, Player*>::iterator it = players.begin();
			while (it != players.end() ){
				Player* p = it->second;
				//Slap them in somewhere.
				p->setPosition(board->cell(0,0), NONE);
				board->cell(0,0)->addPlayer(p);
				p->tellClient( ALL_CLIENTS );
				it++;
			}
		}
	}
}

CLIENT_HANDLER_DIRECTIVE DDServer::connect(CLIENT* c){
	c->userData = NULL;
	std::cout << "A client connected." << std::endl;
	return ACCEPT;
}

void DDServer::disconnect(CLIENT* c){
	std::map<int, Player*>::iterator it;
	if (c->userData != NULL){
		it = players.find(((Player*)c->userData)->playerNum());
		if (it != players.end())
			players.erase(it);
		send( ALL_CLIENTS, (PACKET*)(&P_PLAYERDISCONNECT( ((Player*)c->userData)->playerNum())));
		delete (Player*)c->userData;
		std::cout << "A player disconnected." << std::endl;
	} else
		std::cout << "A client disconnected." << std::endl;
}

void DDServer::receive(CLIENT* c){
	PACKET* p = (PACKET*)c->inBuffer;

	if ( c->userData == NULL ){ //All we are willing to see is a join
		if ( p->type != T_PLAYERJOIN ){
			return;
		}
		//Process the join 
		P_PLAYERJOIN* pj = (P_PLAYERJOIN*)p;
		playernum++;
		c->userData = (void*)new Player(this, playernum, pj->name, pj->model, (void*)c);
		players[playernum] = (Player*)c->userData;


		//Stick the player num into the packet (we ignored it before)
		pj->num = playernum;

		//Tell everyone who they are
		send( ALL_CLIENTS, (PACKET*)pj );

		//And tell the new player who everyone is
		for (std::vector<CLIENT*>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			if ( *it != c ){ //dont send to the newly joined one
				Player* other = (Player*)(*it)->userData;
				pj->num = other->playerNum();
				strcpy(pj->name, other->getName().c_str() );
				strcpy(pj->model, other->getModel().c_str() );
				send(c, (PACKET*)pj);
			}
		}
		//Tell player who he is
		P_YOURPLAYER py(playernum);
		send(c, (PACKET*)(&py));

		//And if there is a board tell him about that
		if (board != NULL){
			board->tellClient(c);
			Player* p = (Player*)c->userData;
			//Slap them in somewhere.
			p->setPosition(board->cell(0,0), NONE);
			board->cell(0,0)->addPlayer((Player*)c->userData);
			p->tellClient( ALL_CLIENTS );
		}
	}

	//Stuff to use in some of the cases..
	Player* pPlayer = ((Player*)c->userData);
	P_CHAT* pc;

	switch (p->type){

		case T_PLAYERJOIN: //Do nothing as they have joined.
			break;

		case T_CHAT:
			pc = (P_CHAT*)p;
			//If the from number matches the player
			if ( pc->player == pPlayer->playerNum() ){
					send( ALL_CLIENTS, (PACKET*)c->inBuffer );
					std::cout << pPlayer->getName() << ": " << pc->str << std::endl;
			} else {
				std::cout << "Player chatted with wrong number!" << std::endl;
			}
			break;
		case T_PLAYERNAME:
			if ( ((P_PLAYERNAME*)p)->player == pPlayer->playerNum() ){
				//It would be wist to see if it is already taken?
				send( ALL_CLIENTS, (PACKET*)c->inBuffer );
				pPlayer->setName( ((P_PLAYERNAME*)p)->name );
			} else {
				std::cout << "Player tried to change someone else's name!" << std::endl;
			}
			break;
		case T_PLAYERMODEL:
			if ( ((P_PLAYERMODEL*)p)->player == pPlayer->playerNum() ){
				//It would be wise to verify that the model is real?
				send( ALL_CLIENTS, (PACKET*)c->inBuffer );
				pPlayer->setModel( ((P_PLAYERMODEL*)p)->model );
			} else {
				std::cout << "Player tried to change someone else's model!" << std::endl;
			}
			break;
		case T_CLIENTSTATE:
			if (board != NULL)
				pPlayer->setState((P_CLIENTSTATE*)p);
			break;


		// @new
		// case that they want to process a server command
		case T_SCOMMAND:

			std::cout << "server command packet receieved" << std::endl;

			// use the Sconsole object to execute this command.  
			processConsoleCmd( ((P_SCOMMAND*)p)->str );


			break;
		}
		// @end new
}




// main function
int main(int argc, char** argv){
	/*if (argc != 2){
		std::cout << "One arg: port to listen on." << std::endl;
		return -1;
	}*/

	////////////////////////////
	//Initialize timing
	////////////////////////////
	int             elapsedTime;
#ifdef WIN32
	INT64	hpcFreq;
	if ( !QueryPerformanceFrequency( (LARGE_INTEGER*)&hpcFreq ) ){
		std::cout << "System does not support high resolution timer" << std::endl;
		return -1;
	}
#else
	struct timeb before;
	struct timeb after;
#endif
	DDServer* d = new DDServer( DEFAULT_PORT );
	while(1){

		if( !d->isPaused() ){

#ifdef WIN32
		//Calculate time since last frame
		static INT64 lastTime;
		INT64 now;
		QueryPerformanceCounter( (LARGE_INTEGER*)&now );
		INT64 hpcElapsed = now - lastTime;
		QueryPerformanceCounter( (LARGE_INTEGER*)&lastTime );
		elapsedTime = hpcElapsed * 1000.0f/(float)hpcFreq;
#else
		static struct timeb lastTime;
		struct timeb now;
		ftime( &now );
		elapsedTime = (now.time - lastTime.time)*1000 + now.millitm - lastTime.millitm;
		ftime( &lastTime );	
#endif
		if (elapsedTime < 1)
			elapsedTime = 1;
		//std::cout << "Elapsed time: " << elapsedTime << std::endl;
		d->tick(elapsedTime);

		} else {// end of if statement
			d->process();
		}

	} // end of while loop 
	return 0;
}
