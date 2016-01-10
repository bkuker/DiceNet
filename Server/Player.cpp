// Player.cpp: implementation of the Player class.
//
//////////////////////////////////////////////////////////////////////

#include "Player.h"
#include "DDServer.h"
#include <math.h>
#include <iostream>

#define SQUASH_POINTS -100
#define PI 3.14159

int Player::totalScore = 0;

//////////////////////////////////////////////////////////////////////
// Vec2 fcns
//////////////////////////////////////////////////////////////////////

Vec2 operator+(Vec2 a, Vec2 b){
	return Vec2(a.x+b.x, a.y+b.y);
}

Vec2 operator-(Vec2 a, Vec2 b){
	return Vec2(a.x-b.x, a.y-b.y);
}

bool operator==(Vec2 a, Vec2 b){
	return a.x==b.x && a.y == b.y;
}

Vec2 operator*(Vec2 a, float k){
	return Vec2(k*a.x, k*a.y);
}

Vec2 operator/(Vec2 a, float k){
	return Vec2(a.x/k, a.y/k);
}

float length(Vec2 a){
	return sqrt( a.x*a.x + a.y*a.y );
}

Vec2 normalize(Vec2 a){
	return a / length(a);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Player::Player(DDServer* _dds, int num, std::string _name, std::string _model, void* _cli_struct ):
dds(_dds),
playernum(num),
model(_model),
name(_name),
score(0),
speed(1.0f),
m_pCell(NULL),
m_pDest(NULL),
cli_struct(_cli_struct)
{
	for(int i = 0; i < 7; i++)
		stats[i] = 0;
}

Player::~Player()
{
	if ( m_pCell != NULL )
		 m_pCell->delPlayer(this);
}

void Player::reset(){
	score = 0;
	for(int i = 0; i < 7; i++)
		stats[i] = 0;

	m_pCell = NULL;
	m_pDest = NULL;
	up = false;
}

int Player::playerNum(){
	return playernum;
}

std::string Player::getName(){
	return name;
}

void Player::setName(std::string n){
	name = n;
}

std::string Player::getModel(){
	return model;
}

void Player::setModel(std::string m){
	model = m;
}

void Player::setState(P_CLIENTSTATE* p){
	//std::cout << "Player " << playernum << " received client state update." << std::endl;
	if (p->control == NONE){
	//	m_pDest = m_pCell;
	}else{
		if (m_pCell->state != ROLLOUT){	//Guard condition for trying to move
			m_pDest = m_pCell->n((CELL_DIR)p->control);
			if (m_pDest == NULL){// || !mayPass(m_pCell->state, m_pDest->state) ){
				m_pDest	= m_pCell;
				destEdge = (CELL_DIR)p->control;
			} else
				destEdge = NONE;
			sendDest( ALL_CLIENTS );
		}
	}
}

void Player::squash(){
	dds->send( ALL_CLIENTS, (PACKET*)&P_CHAT(playernum, "Ouch!") );
	award(SQUASH_POINTS);
}

void Player::setPosition(Cell* cell, CELL_DIR edge){
	m_pCell = cell;
	m_pDest = cell;
	destEdge = NONE;
	if (cell->state == OCCUPIED || cell->state == RISE2)
		up = true;
	else
		up = false;

//	m_v2Position.x = p.x;
//	m_v2Position.y = p.y;
}

void Player::award(int points){
	totalScore += points;
	score += points;
	dds->send( ALL_CLIENTS, (PACKET*)&P_PLAYERSCORE(playernum, score) );
}

void Player::resetScore(){
	score = 0;
	dds->send( ALL_CLIENTS, (PACKET*)&P_PLAYERSCORE(playernum, score) );
}


void Player::tick(int milliseconds){
	if (m_pCell->state == CLEAR && !m_pCell->into)
		up = false;
	if (m_pCell->state == OCCUPIED)
		up = true;
	doMovement(milliseconds);
	//std::cout << "Player " << playernum << " at " << m_v2Position.x << " , " << m_v2Position.y << " " << milliseconds << std::endl;
}

void Player::tellClient(CLIENT* c){
	if (m_pCell!=NULL)
		dds->send( c, (PACKET*)&P_PLAYERWARP(playernum, m_pCell->x, m_pCell->y) );
}

void Player::doMovement(int ms){
	//std::cout << ms << std::endl;
	if ( m_pCell == NULL || m_pDest == NULL)
		return;
	Vec2 dest(0.5f, 0.5f);
	dest.y += m_pDest->y - m_pCell->y;
	dest.x += m_pDest->x - m_pCell->x;
	switch (destEdge)
		{
		case NORTH:
			dest.y += .5;
			break;
		case SOUTH:
			dest.y -= .5;
			break;
		case EAST:
			dest.x += .5;
			break;
		case WEST:
			dest.x -= .5;
			break;
		}

	float d = (ms / CELL_ROLL_TIME) / 1000.0f;
	Vec2 dd = dest - Vec2(m_v2Position.x,m_v2Position.y);
	Vec2 newPos;
	
	if ( length( dd ) > d ){
		//if we are not close to it
		dd = normalize( dd);
		dd = dd * d;
		newPos = Vec2(m_v2Position.x,m_v2Position.y) + dd;
	} else {
		//If we are there we arnt moving
		newPos = dest;
		return;
	}
/*

	//Now take into account a sliding die
	if (m_pCell->state == PUSHOUT && up){
		m_pCell->delPlayer(this);
		m_pCell = m_pCell->n(m_pCell->out);
		m_pCell->addPlayer(this);
	}
*/
	CELL_DIR newDir = NONE;
	Cell* newCell = m_pCell;
	if (newPos.y < 0){
		if ( newCell->n(SOUTH) != NULL ){
			newPos.y += 1;
			newCell = newCell->n(SOUTH);
			newDir = SOUTH;
		} else {
			newPos.y = 0;
		}
	} else 	if (newPos.y > 1){
		if ( newCell->n(NORTH) != NULL ){
			newPos.y -= 1;
			newCell = newCell->n(NORTH);
			newDir = NORTH;
		} else {
			newPos.y = 1;
		}
	} else 	if (newPos.x < 0){
		if ( newCell->n(WEST) != NULL ){
			newPos.x += 1;
			newCell = newCell->n(WEST);
			newDir = WEST;
		} else {
			newPos.x = 0;
		}
	} else 	if (newPos.x > 1){
		if ( newCell->n(EAST) != NULL ){
			newPos.x -= 1;
			newCell = newCell->n(EAST);
			newDir = EAST;
		} else {
			newPos.x = 1;
		}
	}

	//if (m_pCell->into)
	//	return;

	if (m_pCell != newCell){
		//You may move to the new cell if
		BOOL ok = mayPass( m_pCell->state, newCell->state);

		if ( m_pCell->state == PUSHOUT && m_pCell->out == newDir )
			ok = true;


		//std::cout << "Player " << playernum << " trying to move from " << m_pCell->x << "," << m_pCell->y << " to "
		//	<< m_pDest->x << "," << m_pDest->y << std::endl;

		if (ok){
			//free to move
			m_pCell->delPlayer(this);
			newCell->addPlayer(this);
			m_pCell=newCell;
			m_v2Position = newPos;
			sendCell( ALL_CLIENTS );
		} else if ( !up && newCell->state == OCCUPIED ){
			//Try to push
			if (m_pCell->n(newDir)->push(newDir, this) == YES)
			{
				m_pCell->delPlayer(this);
				newCell->addPlayer(this);
				m_pCell=newCell;
				m_v2Position = newPos;
				sendCell( ALL_CLIENTS );
				///MAy not need this?
				sendDest( ALL_CLIENTS );
			} else {
				//may not move
				//std::cout << "Player " << playernum << " hit the edge." << std::endl;
				m_pDest = m_pCell;
				destEdge = newDir;
				sendDest( ALL_CLIENTS );
			}
		} else if ( up && m_pCell->state == OCCUPIED ){
			//Try to roll
			if ( m_pCell->roll(newDir, this) == NO ){
				//may not move
				//std::cout << "Player " << playernum << " hit the edge." << std::endl;
				m_pDest = m_pCell;
				destEdge = newDir;
				sendDest( ALL_CLIENTS );
			}
		} else if ( up && m_pCell->state == ROLLOUT && newDir == m_pCell->out ){
			//If they are going the way the die is rolling then they are ok :)
		} else { //NOT OK
			//may not move
			m_pDest = m_pCell;
			destEdge = newDir;
			sendDest( ALL_CLIENTS );
		}
	} else {
		m_v2Position = newPos;
	}

/*		if (ok)
		{ // Move to the new cell
			m_pCell->delPlayer(this);
			newCell->addPlayer(this);
			m_pCell=newCell;
			m_v2Position = newPos;
		} else if ( up && m_pCell->state == OCCUPIED ){
			m_pCell->roll(newDir, this);
		} else if ( !up && newCell->state == OCCUPIED ){
			if (m_pCell->n(newDir)->push(newDir, this) == YES)
			{
				m_pCell->delPlayer(this);
				newCell->addPlayer(this);
				m_pCell=newCell;
				m_v2Position = newPos;
			}
		}
	} else {
		m_v2Position = newPos;
	}
*/
}

void Player::sendDest(CLIENT* c){
	dds->send( c, (PACKET*)&P_PLAYERDEST(playernum, m_pDest->x, m_pDest->y, destEdge, up) );
}

void Player::sendCell(CLIENT* c){
	dds->send( c, (PACKET*)&P_PLAYERCELL(playernum, m_pCell->x, m_pCell->y) );
}

bool Player::mayPass(CELL_STATE c, CELL_STATE n){
	
	bool ok = false;

	if ( c == RISE1 && n == CLEAR )
		return false;
	if ( c == SINK2 && n == SINK1 ){
		up = true;
		return true;
	}

	if ( !up  ){
		ok = (n == CLEAR || n == RISE1 || n == LIGHTNING || n == SINK2 || ( c == SINK1 && n == SINK2 ) );
	}
	else {

		if (
			( c == RISE1 && n == RISE2 ) || 
			( c == SINK2 && n == SINK1 )
			){
			ok = true;
		}
		
		if (
			( c == n ) ||
			( c == RISE2 && n == OCCUPIED ) ||
			( c == SINK1 && n == OCCUPIED ) ||
			( c == LIGHTNING && n == SINK2 ) ||
			( c == SINK2 && n == RISE1 ) ||
			( c == RISE1 && n == SINK2 ) ||
			( c == SINK1 && n == RISE2 ) ||
			( c == CLEAR && n == SINK2 ) ||
			( c == CLEAR && n == LIGHTNING ) ||
			( c == LIGHTNING && n == CLEAR ) ||
			( c == CLEAR && n == RISE1 ) ||
			( c == LIGHTNING && n == RISE1 ) ||
			( c == RISE1 && n == LIGHTNING ) ||
			( c == SINK2 && n == CLEAR ) ||
			( c == SINK2 && n == LIGHTNING ) ||
			( c == RISE2 && n == SINK1 )
			){
			ok = true;
		}
	}
	return ok;
}