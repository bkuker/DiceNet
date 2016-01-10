// Player.h: interface for the Player class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYER_H__2C37709A_988E_4E45_B6DE_268F71A1B78B__INCLUDED_)
#define AFX_PLAYER_H__2C37709A_988E_4E45_B6DE_268F71A1B78B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "../packets.h"
class Cell;
class Board;
class DDServer;
struct CLIENT;

struct Vec2{
	float x;
	float y;
	Vec2(float _x, float _y){
		x = _x;
		y = _y;
	}
	Vec2(){
		x = 0.0f;
		y = 0.0f;
	}
};

class Player  
{
friend Cell;
friend Board;
friend DDServer;
public:
	Player(DDServer* _dds, int num, std::string name, std::string model, void* cs);
	virtual ~Player();
	
public:
	int playerNum();
	std::string getName();
	void setName(std::string);
	std::string getModel();
	void setModel(std::string);
	int getScore(){ return score; };

	void tellClient(CLIENT* c);
	void sendDest(CLIENT* c);
	void sendCell(CLIENT* c);

	void setState(P_CLIENTSTATE* p);
	void squash();
	void setPosition(Cell* cell, CELL_DIR edge);
	void award(int points);
	void resetScore();
	void tick(int milliseconds);
	void reset();



	void doMovement(int milliseconds);

	const void* cli_struct;
	bool mayPass(CELL_STATE c, CELL_STATE n);


private:
	DDServer* dds;
	int playernum;
	std::string name;
	std::string model;

	int score;
	int stats[7];
	Cell *m_pCell, *m_pDest;
	Vec2 m_v2Position;
	CELL_DIR destEdge;
	bool up;
	float speed;

	static int totalScore;

};

#endif // !defined(AFX_PLAYER_H__2C37709A_988E_4E45_B6DE_268F71A1B78B__INCLUDED_)
