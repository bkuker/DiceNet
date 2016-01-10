////////////////////////////////
//Daemon Dice Packets
////////////////////////////////
#ifndef DAEMON_DICE_PACKETS_H__
#define DAEMON_DICE_PACKETS_H__


#include "packet.h"
#ifdef WIN32
     #include "winsock2.h"
#else
     #include <sys/types.h>
     #include <netinet/in.h>
     #include <inttypes.h>
#endif
#include <string.h>
#define STRLEN	20

#define DEFAULT_PORT 6666

// @new - added two packet types
enum PACKET_TYPES {	T_PLAYERJOIN, T_CHAT, T_CLIENTSTATE, T_REJECT, T_NEWPLAYER, T_CELLFILL, T_PLAYERSTAT,
					T_PLAYERNAME, T_YOURPLAYER, T_PLAYERCELL, T_PLAYERMODEL, T_CELLSTEP, T_PLAYERWARP,
					T_PLAYERSCORE, T_BOARD, T_PLAYERDEST, T_CELLSTATE, T_PLAYERDISCONNECT, T_MSG, T_SCOMMAND, T_SALIAS };
// @end new

enum MSG_TYPES { MSG_LEVEL, MSG_CHAIN, MSG_LONGCHAIN, MSG_GAMEOVER, MSG_TTS };

#ifndef __CELL_DIR__
#define __CELL_DIR__ 
enum CELL_DIR {NORTH, SOUTH, EAST, WEST, NONE}; 
#endif

#ifndef __CELL_STATE__
#define __CELL_STATE__
enum CELL_STATE {CLEAR, LIGHTNING, RISE1, RISE2, OCCUPIED, PUSHOUT, ROLLOUT, SINK1, SINK2, CHAIN};
#endif

//////////////////////////////////////////////////////////////////////////////////////
/////CLIENT PACKETS
//////////////////////////////////////////////////////////////////////////////////////


//State
struct P_CLIENTSTATE{
	PACKET p;
	byte control;
	P_CLIENTSTATE(CELL_DIR _c){
		p.type = T_CLIENTSTATE;
		p.size = sizeof(P_CLIENTSTATE);
		control = _c;
	}
};

//////////////////////////////////////////////////////////////////////////////////////
///SHARED PACKETS
//////////////////////////////////////////////////////////////////////////////////////

//Chat
struct P_CHAT{
	PACKET p;
	byte player;
	char str[253];
	P_CHAT(int _player, const char* _str){
		p.type = T_CHAT;
		p.size = strlen(_str) + 3 + sizeof(PACKET) + sizeof(byte);
		player = _player;
		strncpy(str, _str, 253);
		str[strlen(_str)] = 0;
		str[252] = 0;	//Just to be sure :)
	}
};


//Join
struct P_PLAYERJOIN{
	PACKET p;
	byte num;
	char name[STRLEN];
	char model[STRLEN];
	P_PLAYERJOIN(int _num, const char* _name, const char* _model){
		p.type = T_PLAYERJOIN;
		p.size = sizeof(P_PLAYERJOIN);
		num = _num;
		strncpy(name, _name, STRLEN);
		strncpy(model, _model, STRLEN);
	}
};

//PlayerName
#define NAMELEN 8
struct P_PLAYERNAME{
	PACKET p;
	byte player;
	char name[NAMELEN];
	P_PLAYERNAME( int _player, char* _name ){
		p.type = T_PLAYERNAME;
		p.size = sizeof(P_PLAYERNAME);
		player = _player;
		strncpy(name, _name, NAMELEN);
		name[NAMELEN-1]=0;
	}
};

//PlayerModel
struct P_PLAYERMODEL{
	PACKET p;
	byte player;
	char model[STRLEN];
	P_PLAYERMODEL(int _player, char* _model){
		p.type = T_PLAYERMODEL;
		p.size = sizeof(P_PLAYERMODEL);
		player = _player;
		strncpy(model, _model, STRLEN);
	}

};

// @new

//SCOMMAND - this is now identical to chat except type 
struct P_SCOMMAND{
	PACKET p;
	byte player;
	char str[253];
	P_SCOMMAND(int _player, const char* _str){
		p.type = T_SCOMMAND;
		p.size = strlen(_str) + 3 + sizeof(PACKET) + sizeof(byte);
		player = _player;
		strncpy(str, _str, 253);
		str[strlen(_str)] = 0;
		str[252] = 0;	//Just to be sure :)
	}
};

// @end new

//////////////////////////////////////////////////////////////////////////////////////
///SERVER PACKETS
//////////////////////////////////////////////////////////////////////////////////////
//Server message
struct P_MSG{
	PACKET p;
	byte type;
	int p1;
	int p2;
	P_MSG(byte _type, int _p1=0, int _p2=0){
		p.type = T_MSG;
		p.size = sizeof(P_MSG);
		type = _type;
		p1 = htonl(_p1);
		p2 = htonl(_p2);
	}
};

//Player stat
struct P_PLAYERSTAT{
	PACKET p;
	byte player;
	int stats[7];
	P_PLAYERSTAT( int _player, int *_stats ){
		p.type = T_PLAYERSTAT;
		p.size = sizeof(P_PLAYERSTAT);
		player = _player;
		for (int i = 0; i < 7; i++)
			stats[i] = htonl(_stats[i]);
	}
};

//Reject
struct P_REJECT{
	PACKET p;
	char reason[STRLEN];
	P_REJECT(char* _reason){
		p.type = T_REJECT;
		p.size = sizeof(P_REJECT);
		strncpy(reason, _reason, STRLEN);
	}
};

//Disconnect
struct P_PLAYERDISCONNECT{
	PACKET p;
	byte player;
	P_PLAYERDISCONNECT(int _player){
		p.type = T_PLAYERDISCONNECT;
		p.size = sizeof(P_PLAYERDISCONNECT);
		player = _player;
	}
};


//YourPlayer
struct P_YOURPLAYER{
	PACKET p;
	byte player;
	P_YOURPLAYER(int _player){
		p.type = T_YOURPLAYER;
		p.size = sizeof(P_YOURPLAYER);
		player = _player;
	}
};

//PlayerPos
struct P_PLAYERCELL{
	PACKET p;
	byte player;
	byte cellx;
	byte celly;
	P_PLAYERCELL(int _player, int _cx, int _cy){
		p.type = T_PLAYERCELL;
		p.size = sizeof(P_PLAYERCELL);
		player = _player;
		cellx = _cx;
		celly = _cy;
	}
};

//PlayerPos
struct P_PLAYERWARP{
	PACKET p;
	byte player;
	byte cellx;
	byte celly;
	P_PLAYERWARP(int _player, int _cx, int _cy){
		p.type = T_PLAYERWARP;
		p.size = sizeof(P_PLAYERWARP);
		player = _player;
		cellx = _cx;
		celly = _cy;
	}
};

//PlayerState
struct P_PLAYERDEST{
	PACKET p;
	byte player;
	byte cellx;
	byte celly;
	byte edge;
	byte up;
	P_PLAYERDEST(int _player, int _cx, int _cy, CELL_DIR _e, bool _up){
		p.type = T_PLAYERDEST;
		p.size = sizeof(P_PLAYERDEST);
		player = _player;
		cellx = _cx;
		celly = _cy;
		edge = _e;
		up = _up?~0:0;
	}
};

//PlayerScore
struct P_PLAYERSCORE{
	PACKET p;
	byte player;
	int score;
	P_PLAYERSCORE(int _player, int _score){
		p.type = T_PLAYERSCORE;
		p.size = sizeof(P_PLAYERSCORE);
		player = _player;
		score = htonl(_score);
	}
};

//Board
struct P_BOARD{
	PACKET p;
	byte size;
	P_BOARD(int _size){
		p.type = T_BOARD;
		p.size = sizeof(P_BOARD);
		size = _size;
	}
};

//Cell
struct P_CELLFILL{
	PACKET p;
	byte cx;
	byte cy;
	byte up;
	byte east;
	byte south;
	P_CELLFILL(int _x, int _y, int _r1, int _r2, int _r3){
		p.size = sizeof(P_CELLFILL);
		p.type = T_CELLFILL;
		cx = _x;
		cy = _y;
		up = _r1;
		east = _r2;
		south = _r3;
	}
};

struct P_CELLSTATE{
	PACKET p;
	byte cx;
	byte cy;
	byte param;
	byte state;
	P_CELLSTATE(int _x, int _y, CELL_STATE _s){
		p.size = sizeof(P_CELLSTATE);
		p.type = T_CELLSTATE;
		cx = _x;
		cy = _y;
		state = _s;
		param = 0;
	};
	P_CELLSTATE(int _x, int _y, CELL_STATE _s, int _param){
		p.size = sizeof(P_CELLSTATE);
		p.type = T_CELLSTATE;
		cx = _x;
		cy = _y;
		state = _s;
		param = _param;
	}

};

struct P_CELLSTEP{
	PACKET p;
	byte cx;
	byte cy;
	P_CELLSTEP(int _x, int _y){
		p.size = sizeof(P_CELLSTEP);
		p.type = T_CELLSTEP;
		cx = _x;
		cy = _y;
	}
};


#endif
