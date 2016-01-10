// Cell.h: interface for the Cell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CELL_H__F9C31265_C262_4B01_8F1F_B3171908F93A__INCLUDED_)
#define AFX_CELL_H__F9C31265_C262_4B01_8F1F_B3171908F93A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DDClient.h"
#include "Animated.h"
#include "Die.h"
#include "Lightning.h"
#include "Context.h"

#ifndef __CELL_DIR__
#define __CELL_DIR__ 
enum CELL_DIR {NORTH, SOUTH, EAST, WEST, NONE}; 
#endif

#ifndef __CELL_STATE__
#define __CELL_STATE__
enum CELL_STATE {CLEAR, LIGHTNING, RISE1, RISE2, OCCUPIED, PUSHOUT, ROLLOUT, SINK1, SINK2};
#endif

enum CELL_RESULT {NO, YES};

#define CELL_SINK_TIME 9.0f
#define CELL_RISE_TIME 3.0f
#define CELL_ROLL_TIME 0.2f
#define CELL_PUSH_TIME 0.2f
#define CELL_LIGHTNING_TIME 0.5f

class Board;
class Player;
class Info;

class Cell : public Animated, public Renderable
{
friend Board;
friend Player;
friend Info;
public:
	Cell(DDClient* dxs, int x, int y);
	virtual ~Cell();

public:
	CELL_RESULT fill(int up, int east, int south);
	void setState( CELL_STATE s, int param );
	void reset();
	void render(Context* c);
	void tick(int milliseconds);
	void draw(){};
	Cell* n(CELL_DIR);
	void addPlayer( Player* );
	void delPlayer( Player* );
	void step();

private: //Transition helpers
	void makeDie(int up, int east, int south);
	void rotate(CELL_DIR);

private: //State info
	CELL_STATE state;
	int n_up, n_east, n_south, x, y;
	CELL_DIR in, out;
	bool into;
	float time;
	bool checkMatch;
	bool rollSound;
	Player* m_pWhoSunk;
	int chain;

private: //Graphic info
	Die* die;
	Lightning* lightning;

private:
	Cell *north, *south, *east, *west;
	DDClient* ddc;
	SOUND_REF snd_roll, snd_slide, snd_lightning;
	std::set<Player*> players;

	//Static match count helpers
	static std::set<Cell*> visited;
	static int matchCount;
	static int dieCount;
	static Player* sinker;

	//Static one stuff
	static bool oneFall;
	static Cell* oneStart;
};

#endif // !defined(AFX_CELL_H__F9C31265_C262_4B01_8F1F_B3171908F93A__INCLUDED_)
