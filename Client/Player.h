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
#include "Animated.h"
#include "Renderable.h"
class AnimatedMd2;
class DXSystem;
class Cell;
class Text;
class Info;
class Scores;
class Summary;

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

#define STATSIZE 7

class Player : public Renderable, public Animated
{
friend Cell;
friend Info;
friend Scores;
friend Summary;
public:
	Player(DXSystem* dxs, int num, std::string name, std::string model);
	virtual ~Player();
	
public:
	int playerNum();
	std::string getName();
	void setName(std::string);
	void setModel(std::string);
	void setCell(Cell* cell);
	void setCellWarp(Cell* cell);
	void setDest(Cell* cell, CELL_DIR edge, bool up);
	void setScore(int s);
	void setStats(int*);
	int getScore() const;
	int getNum() const;
	void reset();

//Graphics
public:
	void render(Context* c);
	void draw(){};
	void onResetDevice();
	void tick(int milliseconds);

private:
	int playernum;
	int score;
	int stats[STATSIZE];
	std::string name;
	std::string modelname;

	Cell *m_pCell, *m_pDest;
	D3DXVECTOR3 m_v3Color1, m_v3Color2;
	D3DXVECTOR2 m_v2Position;
	CELL_DIR facing, destEdge;
	bool up;

	///Graphics
	D3DMATERIAL8 m_mtrl;
	AnimatedMd2* model;
	Text* p_TScore;


};

#endif // !defined(AFX_PLAYER_H__2C37709A_988E_4E45_B6DE_268F71A1B78B__INCLUDED_)
