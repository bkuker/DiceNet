// Scores.cpp: implementation of the Scores class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Scores.h"
#include "Context.h"
#include "Text.h"

D3DXMATRIX Scores::s_mD1;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scores::Scores(DDClient* _ddc):
ddc(_ddc),
UIElement(_ddc)
{
	D3DXMatrixTranslation( &s_mD1,0, 1.0f,0);
}

Scores::~Scores()
{

}

struct Scores::compare {
    bool operator() (const Player *a, const Player *b) const {
        return (a->getScore() > b->getScore()) || ((a->getScore() == b->getScore()) && (a->getNum() > b->getNum()));
    }
};

void Scores::score_change() {
	scores.clear();
	std::map<int, Player*>::iterator i = ddc->players.begin();
	while ( i != ddc->players.end() ){
		scores.insert(i->second);
		i++;
	}
}

void Scores::render(Context* c){
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	
	std::set<Player*, compare>::iterator i = scores.begin();
	while ( i != scores.end() ){
		(*i)->p_TScore->render(c);
		c->m_mspWorld->MultMatrixLocal( &s_mD1 );
		i++;
	}

	c->m_mspWorld->Pop();
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}