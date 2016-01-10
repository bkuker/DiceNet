// Cell.cpp: implementation of the Cell class.
//
//////////////////////////////////////////////////////////////////////
#include "Cell.h"
#include "math.h"
#include "Player.h"
#include "DDServer.h"
#include <iostream>

CELL_DIR c(CELL_DIR); //returns the opposite direction

std::set<Cell*> Cell::visited;
int Cell::matchCount = 0;
int Cell::fullCount = 0;
Player* Cell::sinker = NULL;

bool Cell::oneFall = false;
Cell* Cell::oneStart = NULL;

Cell* Cell::firstFree = NULL;
int Cell::freeCount = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cell::Cell(DDServer* _dds, int _x, int _y):
	dds(_dds),
	x(_x),
	y(_y),
	checkMatch(false),
	m_pWhoSunk(NULL),
	chain(0)
{
	std::cout << "Created cell " << x << "," << y << std::endl;
	

	//Get me into the free list
	nextFree = this;
	prevFree = this;
	state = CLEAR;
	setFree();

	reset();
}

Cell::~Cell()
{
	std::set<Player*>::iterator i;
	for (i = players.begin(); i != players.end(); i++ ){
		(*i)->m_pCell = NULL;
		(*i)->m_pDest = NULL;
	}
}

void Cell::setFree(){
	freeCount++;
	if ( firstFree == NULL )
		firstFree = this;
	else {
		nextFree = firstFree->nextFree;
		firstFree->nextFree = this;
		nextFree->prevFree = this;
		prevFree = firstFree;
		firstFree = this;
	}
	//std::cout << "Cell " << x << "," << y << " freed (" << freeCount << ") free" << std::endl;
}

void Cell::unFree(){
	freeCount--;
	prevFree->nextFree = nextFree;
	nextFree->prevFree = prevFree;
	if ( firstFree == this )
		firstFree = nextFree;
	if ( firstFree == this )
		firstFree = NULL;
	prevFree = this;
	nextFree = this;
	//std::cout << "Cell " << x << "," << y << " filld (" << freeCount << ") free" << std::endl;
}

void Cell::addPlayer( Player* pe)
{
	players.insert(pe);
}

void Cell::delPlayer( Player* pe)
{
	std::set<Player*>::iterator i = players.find(pe);
	if (i != players.end())
		players.erase(i);
}


void Cell::reset()
{
	time = 0;
	if ( state != CLEAR )
		setFree();
	state = CLEAR;
	into = false;
	chain = 0;
	m_pWhoSunk = NULL;
	checkMatch = false;
	dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTATE(x,y,state));
}

void Cell::tellClient(CLIENT* c){
	if (state != CLEAR){
		dds->send(c, (PACKET*)&P_CELLFILL(x, y, n_up, n_east, n_south) );
		dds->send(c, (PACKET*)&P_CELLSTATE(x, y, state) );
	}

	std::set<Player*>::iterator i;
	for (i = players.begin(); i != players.end(); i++ )
		(*i)->tellClient(c);
}

void Cell::tick(int ms)
{
	if (oneFall && this == oneStart)
	{
		oneStart = NULL;
		oneFall = false;
		//std::cout << "One stop." << std::endl;
	}
	if (time < 0)
		time = 0;
	switch (state){
	case CLEAR:
		break;
	case LIGHTNING:
		time = time + ms / CELL_LIGHTNING_TIME / 1000;
		if (time >= 1.0f)
			step();
		break;
    case RISE1:
		time = time + ms / CELL_RISE_TIME / 1000;
		if (time >= 0.5f)
			step();
        break;
	case RISE2:
		time = time + ms / CELL_RISE_TIME / 1000;
		if (time >= 1.0f)
			step();
        break;
	case OCCUPIED:
		if ( n_up == 1 && oneFall && this != oneStart)
		{
			m_pWhoSunk = oneStart->m_pWhoSunk;
			m_pWhoSunk->award(1);
			sink(0);
		}
		if ( checkMatch )
		{	
			matchCount = 0;
			visited.clear();
			sinker = m_pWhoSunk;
			matchCheck();
			sinker = NULL;
			checkMatch = false;
		}
        break;
	case ROLLOUT:
		time = time + ms / CELL_ROLL_TIME / 1000;
		if (time >= 1.0f)
			step();
		break;
	case PUSHOUT:
		time = time + ms / CELL_PUSH_TIME / 1000;
		if (time >= 1.0f)
			step();
        break;
	case SINK1:
		time = time + ms / CELL_SINK_TIME / 1000;
		if (time >= 0.5f)
			step();
        break;
	case SINK2:
		time = time + ms / CELL_SINK_TIME / 1000;
		if (time >= 1.0f)
			step();
        break;
	}
}

void Cell::step()
{
	//std::cout << "Cell " << x << "," << y << " Stepped" << std::endl;
	Cell* no = NULL;
	int e,s,u;
    float t;
	CELL_STATE st;
	std::set<Player*>::iterator i;

	switch (state){
	case CLEAR:
		break;
	case LIGHTNING:
		state = RISE1;
		time=0;
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTEP(x,y) );
		break;
    case RISE1:
		for (i = players.begin(); i != players.end(); i++ ){
			(*i)->up = true;
			(*i)->sendDest(ALL_CLIENTS);
		}
		state = RISE2;
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTEP(x,y) );
        break;
	case RISE2:
		state = OCCUPIED;
		fullCount++;
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTEP(x,y) );
		time = 0;
        break;
	case OCCUPIED:
        break;
	case ROLLOUT:
		///Move players to next cell
		for (i = players.begin(); i != players.end(); i++ ){
			if ( (*i)->up )
			{
				n(out)->players.insert(*i);
				(*i)->m_pCell = n(out);
				//(*i)->m_v2Position.y = 0.5f;
				//(*i)->m_v2Position.x = 0.5f;
				switch (out){
				case NORTH:
					(*i)->m_v2Position.y = 0;
					break;
				case SOUTH:
					(*i)->m_v2Position.y = 1;
					break;
				case EAST:
					(*i)->m_v2Position.x = 0;
					break;
				case WEST:
					(*i)->m_v2Position.x = 1;
					break;
				}
				(*i)->sendCell(ALL_CLIENTS);
			}
		}
		players.clear();

		rotate(out);
	case PUSHOUT:
		//squish'em
		for (i = n(out)->players.begin(); i != n(out)->players.end(); i++ ){
			if ( !((*i)->up)){
				(*i)->squash();
				addPlayer(*i);
				(*i)->m_pCell = this;
				(*i)->m_pDest = this;
				(*i)->destEdge = NONE;
				dds->send(ALL_CLIENTS, (PACKET*)&P_PLAYERWARP((*i)->playernum, x, y) );
			}
		}
		//Now remove all the players in here from n(out)
		//I know it might make an unneeded call or 2
		for (i = players.begin(); i != players.end(); i++ ){
			n(out)->delPlayer(*i);
		}


		no = n(out);
		//Store neighbor vars
		t = no->time;
		e = no->n_east;
		u = no->n_up;
		s = no->n_south;
		st = no->state;
		
		//set neighbor 
		no->state = OCCUPIED;
		no->n_east = n_east;
		no->n_south = n_south;
		no->n_up = n_up;
		no->time = 0;
		no->into = false;
		no->checkMatch = true;
		no->m_pWhoSunk = m_pWhoSunk;

		no->chain = 0;
		chain = 0;
		checkMatch = false;
		m_pWhoSunk = NULL;

		//Unfree the one we are going into only if it is clear
		if (st == CLEAR)
			no->unFree();
		//set my vars
		if ( st == RISE1 || st == RISE2 || st == LIGHTNING){
			state = st;
			time = t;
			n_up = u;
			n_east = e;
			n_south = s;
			into = false;
		} else {
			state = CLEAR;
			into = false;
			setFree();
		}
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTEP(x,y) );
        break;
	case SINK1:
		for (i = players.begin(); i != players.end(); i++ ){
			(*i)->up = false;
			(*i)->sendDest(ALL_CLIENTS);
		}
		state = SINK2;
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTEP(x,y) );
        break;
	case SINK2:
		setFree();
		state = CLEAR;
		m_pWhoSunk = NULL;
		chain = 0;
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTEP(x,y) );
        break;
	}
}

CELL_RESULT Cell::roll(CELL_DIR d, Player* p)
{
	if ( n(d) == NULL )
		return NO;
	if ((
			n(d)->state != CLEAR &&
			n(d)->state != SINK2 &&
			n(d)->state != LIGHTNING &&
			n(d)->state != RISE1
		)||
		state != OCCUPIED
		|| n(d)->into)
		return NO;
	time=0;
	state = ROLLOUT;
	dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTATE(x,y,state,d));
	n(d)->into = true;
	out = d;
	n(d)->in = c(d);
	m_pWhoSunk = p;

	//for all players in n(out) set the dest to n(out)->n(out), none
	std::set<Player*>::iterator i;
	for (i = n(out)->players.begin(); i != n(out)->players.end(); i++ ){
		if (n(out)->n(out) != NULL && (*i)->mayPass(n(out)->state, n(out)->n(out)->state) ){
		(*i)->m_pDest = n(out)->n(out);
		(*i)->destEdge = NONE;
		} else {
			(*i)->m_pDest = n(out);
			(*i)->destEdge = out;
		}
		dds->send(ALL_CLIENTS, (PACKET*)&P_PLAYERDEST( (*i)->playernum, (*i)->m_pDest->x, (*i)->m_pDest->y, (*i)->destEdge, 0 ));
	}

	//for all players here make the dest out.
	for (i = players.begin(); i != players.end(); i++ ){
		(*i)->m_pDest = n(out);
		(*i)->destEdge = NONE;
		(*i)->sendDest(ALL_CLIENTS);
	}


	return YES;
}

CELL_RESULT Cell::push(CELL_DIR d, Player* p)
{
	if ( n(d) == NULL )
		return NO;
	if ((
			n(d)->state != CLEAR &&
			n(d)->state != SINK2 &&
			n(d)->state != LIGHTNING &&
			n(d)->state != RISE1
		)||
		state != OCCUPIED
		|| n(d)->into)
		return NO;
	time=0;
	state = PUSHOUT;
	dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTATE(x,y,state,d));
	n(d)->into = true;
	out = d;
	n(d)->in = c(d);
	m_pWhoSunk = p;
	//std::cout << "Push " << x << "," << y << " by player " << p->playernum << std::endl;

	//for all players in n(out) set the dest to n(out)->n(out), none
	std::set<Player*>::iterator i;
	for (i = n(out)->players.begin(); i != n(out)->players.end(); i++ ){
		if (n(out)->n(out) != NULL && (*i)->mayPass(n(out)->state, n(out)->n(out)->state) ){
		(*i)->m_pDest = n(out)->n(out);
		(*i)->destEdge = NONE;
		} else {
			(*i)->m_pDest = n(out);
			(*i)->destEdge = out;
		}
		dds->send(ALL_CLIENTS, (PACKET*)&P_PLAYERDEST( (*i)->playernum, (*i)->m_pDest->x, (*i)->m_pDest->y, (*i)->destEdge, 0 ));
	}

	//for all players here make the dest out.
	for (i = players.begin(); i != players.end(); i++ ){
		if ( (*i)->up ){
			(*i)->m_pDest = n(out);
			(*i)->destEdge = NONE;
			(*i)->sendDest(ALL_CLIENTS);
		}
	}


	return YES;
}

CELL_RESULT Cell::fill()
{
	if (state == CLEAR){
		//std::cout << "Cell " << x << "," << y << " filled." << std::endl;
		makeDie();
		state = LIGHTNING;
		unFree();
		time = 0;
		chain = 0;
		return YES;
	} else
		return NO;
}

void Cell::makeDie()
{
	n_up = 4;
	n_east = 5;
	n_south = 1;
	//Random rotate(s)

	int r1 = rand() * 6 / RAND_MAX;
	int r2 = rand() * 4 / RAND_MAX;
	int r3 = rand() * 4 / RAND_MAX;

	switch (r1){
	case 0:
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		rotate((CELL_DIR)r2);
		break;
	case 5:
		rotate(NORTH);
		rotate(NORTH);
		break;
	}
	if (r3 > 0){
		rotate(NORTH);
		for (int i = 0; i < r3; i++){
			rotate(EAST);
		}
		rotate(SOUTH);
	}

	dds->send(ALL_CLIENTS, (PACKET*)&P_CELLFILL(x, y, n_up, n_east, n_south) );
}

CELL_RESULT Cell::sink(int c)
{
	if ( state == OCCUPIED )
	{
		fullCount--;
		state=SINK1;
		time=0;
		m_pWhoSunk->stats[n_up]++;
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTATE(x,y,state,m_pWhoSunk->playerNum()));
	}
	
	if ( n_up != 1 ){
		std::set<Cell*>::iterator i = visited.find(this);
		if ( i != visited.end() )
			return YES;
		visited.insert(this);
		
		dds->send(ALL_CLIENTS, (PACKET*)&P_CELLSTATE(x,y,CHAIN,m_pWhoSunk->playerNum()));
		if ( state == SINK1 || state == SINK2){
			time = time - 0.2f;
			//if this puts it back into SINK1 time fix it.
			if ( time < 0.5f && state == SINK2){
				state = SINK1;
				std::set<Player*>::iterator i;
				for (i = players.begin(); i != players.end(); i++ )
					(*i)->up = true;
			}
		}
		for (int x = 0; x < 4; x++)
		{
			if (n((CELL_DIR)x) != NULL &&
				n((CELL_DIR)x)->n_up == n_up &&
					(n((CELL_DIR)x)->state == OCCUPIED ||
					 n((CELL_DIR)x)->state == SINK1 ||
					 n((CELL_DIR)x)->state == SINK2
					)
				)
			{	
				chain = c;
				n((CELL_DIR)x)->sink(c);
			}
		}
	}
	return YES;
}

void Cell::rotate(CELL_DIR d)
{
	int o_up = n_up;
	int o_east = n_east;
	int o_south = n_south;
	switch (d){
	case NORTH:
		n_up = o_south;
		n_south = 7 - o_up;
		break;	
	case SOUTH:
		n_up = 7 - o_south;
		n_south = o_up;
		break;	
	case EAST:
		n_east = o_up;
		n_up = 7-o_east;
		break;	
	case WEST:
		n_up = o_east;
		n_east = 7 - o_up;
		break;
	case NONE:
		break;
	}
}

void Cell::matchCheck()
{
	static int max;
	m_pWhoSunk = sinker;
	if (n_up != 1)
	{
		std::set<Cell*>::iterator i = visited.find(this);
		if ( i != visited.end() )
			return;
		visited.insert(this);
		matchCount++;
		if (checkMatch)
			max = 0;
		if ( chain > max )
			max = chain;
		for (int x = 0; x < 4; x++)
		{
			if ( n((CELL_DIR)x) != NULL &&
				n((CELL_DIR)x)->n_up == n_up
				)
			{
				if ( n((CELL_DIR)x)->state == OCCUPIED )
					n((CELL_DIR)x)->matchCheck();
				else if ( 
					n((CELL_DIR)x)->state == SINK1 ||
					n((CELL_DIR)x)->state == SINK2
					)
				{
					n((CELL_DIR)x)->matchCheck();
				}
			}
		}
		if (matchCount >= n_up && checkMatch)
		{
			chain = max + 1;
			m_pWhoSunk->award( chain * matchCount * n_up );
			
			//Keep track of stats
			if ( chain > m_pWhoSunk->stats[0] )
				m_pWhoSunk->stats[0] = chain-1;

			if (chain > 1){
				dds->send(ALL_CLIENTS, (PACKET*)&P_MSG(MSG_CHAIN, chain-1, m_pWhoSunk->playerNum()));
			}
			visited.clear();
			sink(chain);
		}

	} else {
		//do one check
		for (int x = 0; x < 4; x++)
		{
			if (n((CELL_DIR)x) != NULL && n((CELL_DIR)x)->state == SINK1 && n((CELL_DIR)x)->n_up != 1 ||
				n((CELL_DIR)x) != NULL && n((CELL_DIR)x)->state == SINK2 && n((CELL_DIR)x)->n_up != 1)
			{
				oneFall = true;
				oneStart = this;
			}
		}
	}
	
}

Cell* Cell::n(CELL_DIR d)
{
	switch (d){
	case NORTH:
		return north;
		break;	
	case SOUTH:
		return south;
		break;	
	case EAST:
		return east;
		break;	
	case WEST:
		return west;
		break;
	default:
		return NULL;
	}

}

CELL_DIR c(CELL_DIR d){
	switch (d){
	case NORTH:
		return SOUTH;
		break;	
	case SOUTH:
		return NORTH;
		break;	
	case EAST:
		return WEST;
		break;	
	case WEST:
		return EAST;
		break;
	default:
		return NORTH;
	}

}