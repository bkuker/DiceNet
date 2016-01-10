// Cell.cpp: implementation of the Cell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cell.h"
#include "math.h"
#include "Player.h"

CELL_DIR c(CELL_DIR); //returns the opposite direction

std::set<Cell*> Cell::visited;
int Cell::matchCount = 0;
int Cell::dieCount = 0;
Player* Cell::sinker = NULL;

bool Cell::oneFall = false;
Cell* Cell::oneStart = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cell::Cell(DDClient* _dxs, int _x, int _y):
	Renderable(_dxs),
	ddc(_dxs),
	x(_x),
	y(_y),
	snd_roll(dxs->loadSound("sounds/roll.wav")),
	snd_slide(dxs->loadSound("sounds/slide.wav")),
	snd_lightning(dxs->loadSound("sounds/lightning.wav")),
	die(NULL),
	lightning(NULL),
	checkMatch(false),
	m_pWhoSunk(NULL),
	chain(0),
	rollSound(false)
{
	reset();
}

Cell::~Cell()
{
	safeDelete(die);
	safeDelete(lightning);
	dxs->releaseSound(snd_roll);
	dxs->releaseSound(snd_slide);
	dxs->releaseSound(snd_lightning);
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

void Cell::setState( CELL_STATE s, int param ){
	//dxs->debugprintf("Cell %d, %d got state %d", x, y, s);
	std::set<Player*>::iterator i;
	
	if ( s != CHAIN ) //So CHAIN isnt really a state, but this seemes
		state = s;			//liks a good place to send it, eh?

	switch (s){
	case CLEAR:
		safeDelete(die);
		safeDelete(lightning);
		for (i = players.begin(); i != players.end(); i++ )
			(*i)->up = false;
		break;
	case LIGHTNING:
		time = 0;
		break;
	case RISE1:
		time = 0;
		break;
	case RISE2:
		break;
	case OCCUPIED:
		break;
	case PUSHOUT:
		out = (CELL_DIR)param;
		time = 0;
		dxs->playSound(snd_slide);
		break;
	case ROLLOUT:
		time = 0;
		out = (CELL_DIR)param;
		break;
	case SINK1:
		m_pWhoSunk = ddc->player( param );
		time = 0;
		break;
	case SINK2:
		break;
	case CHAIN:
		time = time - 0.2f;
		//if this puts it back into SINK1 time fix it.
		if ( time < 0.5f && state == SINK2){
			state = SINK1;
			std::set<Player*>::iterator i;
			for (i = players.begin(); i != players.end(); i++ )
				(*i)->up = true;
		}
		m_pWhoSunk = ddc->player( param );
		//dxs->debugprintf("Got Chain!");
		break;
	}

	if ( state != LIGHTNING )
		safeDelete(lightning);
}


void Cell::reset()
{
	safeDelete(die);
	safeDelete(lightning);
	time = 0;
	state = CLEAR;
	into = false;
	chain = 0;
	m_pWhoSunk = NULL;
	checkMatch = false;

}

void Cell::render(Context* c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	if (die != NULL)
	{
		if ( m_pWhoSunk != NULL && (state == SINK1 || state == SINK2) )
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			pd3dDevice->SetMaterial( &(m_pWhoSunk->m_mtrl) );
			die->render(c);
			pd3dDevice->SetMaterial( &mtrl );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		}
		else
		{
			die->render(c);
			//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			//pd3dDevice->SetMaterial( &mtrl );
		}
	}
	if (lightning != NULL)
		lightning->render(c);
	for (std::set<Player*>::iterator i = players.begin(); i != players.end(); i++ )
			(*i)->render(c);
	c->m_mspWorld->Pop();
}


void Cell::tick(int ms)
{
	if (time < 0)
		time = 0;
	switch (state){
	case CLEAR:
		break;
	case LIGHTNING:
		time = time + ms / CELL_LIGHTNING_TIME / 1000;
		lightning->tick(ms);
		break;
    case RISE1:
		time = time + ms / CELL_RISE_TIME / 1000;
		die->rise(time);
        break;
	case RISE2:
		time = time + ms / CELL_RISE_TIME / 1000;
		die->rise(time);
        break;
	case OCCUPIED:
		if ( players.size() > 0)
			die->rise(0.9f);
		else
			die->rise(1);
        break;
	case ROLLOUT:
		time = time + ms / CELL_ROLL_TIME / 1000;
		die->roll(out, time);
		if (time >= 1.0f && !rollSound){
			dxs->playSound(snd_roll);
			rollSound = true;
		}
		break;
	case PUSHOUT:
		time = time + ms / CELL_PUSH_TIME / 1000;
		die->slide(out, time);
        break;
	case SINK1:
		time = time + ms / CELL_SINK_TIME / 1000;
		die->rise(1.0f-time);
        break;
	case SINK2:
		time = time + ms / CELL_SINK_TIME / 1000;
		die->rise(1.0f-time);
        break;
	}
}

void Cell::step()
{
	Cell* no = NULL;
	int e,s,u;
	Die* d;
	Lightning* l;
    float t;
	CELL_STATE st;

	switch (state){
	case CLEAR:
		break;
	case LIGHTNING:
		if (lightning != NULL){
			delete lightning;
			lightning = NULL;
		}
		state = RISE1;
		time=0;
		break;
    case RISE1:

		state = RISE2;
        break;
	case RISE2:
		state = OCCUPIED;
        break;
	case OCCUPIED:
        break;
	case ROLLOUT:
		if (!rollSound)
			dxs->playSound(snd_roll);
		rollSound = false;
		rotate(out);
		die->rotate(out);
	case PUSHOUT:
		die->rise(1);
		no = n(out);
		//Store neighbor vars
		t = no->time;
		e = no->n_east;
		u = no->n_up;
		s = no->n_south;
		l = no->lightning;
		d = no->die;
		st = no->state;
		
		//set neighbor 
		no->state = OCCUPIED;
		no->n_east = n_east;
		no->n_south = n_south;
		no->n_up = n_up;
		no->die = die;
		no->time = 0;
		no->into = false;
		no->checkMatch = true;
		no->lightning = NULL;
		no->m_pWhoSunk = m_pWhoSunk;

		no->chain = 0;
		chain = 0;
		checkMatch = false;
		m_pWhoSunk = NULL;

		//set my vars
		if ( st == RISE1 || st == RISE2 || st == LIGHTNING){
			state = st;
			die = d;
			lightning = l;
			time = t;
			n_up = u;
			n_east = e;
			n_south = s;
			//into = false;
		} else {
			if ( l != NULL )
				delete l;
			if ( d != NULL )
				delete d;
			die = NULL;
			state = CLEAR;
			//into = false;
		}
        break;
	case SINK1:
		state = SINK2;
        break;
	case SINK2:
		delete die;
		die = NULL;
		state = CLEAR;
        break;
	}
	//dxs->debugprintf("Cell %i,%i step() to %i.", x, y, state);
}

CELL_RESULT Cell::fill(int up, int east, int south)
{
	//dxs->debugprintf("Cell %d, %d filled with %d,%d,%d", x, y, up, east, south);	

	//if ( state != CLEAR ){
		safeDelete(die);
		safeDelete(lightning);
	//}
	makeDie(up, east, south);
	state = LIGHTNING;
	dxs->playSound(snd_lightning);
	lightning = new Lightning(dxs);
	time = 0;
	dieCount++;
	chain = 0;
	return YES;
}

void Cell::makeDie(int up, int east, int south)
{
	/*if ( die != NULL ){
		dxs->debugprintf("Cell %i,%i has die object on makeDie() call!", x, y);
		safeDelete(die);
	}*/
	die = new Die(dxs);
	n_up = 4;
	n_east = 5;
	n_south = 1;

	switch (up){
	case 6:
		die->rotate(NORTH);
		rotate(NORTH);
	case 3:
		die->rotate(NORTH);
		rotate(NORTH);
	case 1:
		die->rotate(NORTH);
		rotate(NORTH);
		break;
	case 5:
		die->rotate(WEST);
		rotate(WEST);
		break;
	case 2:
		die->rotate(EAST);
		rotate(EAST);
		break;
	}
	die->rotate(WEST);
	rotate(WEST);
	while( n_up != east ){
		die->rotate(SOUTH);
		rotate(SOUTH);
	}
	die->rotate(EAST);
	rotate(EAST);
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