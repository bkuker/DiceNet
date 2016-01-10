// Summary.cpp: implementation of the Summary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Summary.h"
#include "Text.h"
#include "Scores.h"
#include <set>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Summary::Summary(DDClient* ddc, Scores* sc):Renderable(ddc)
{
	//Find all the max scores
	int max[8];
	for (int j = 0; j < 8; j++){
		max[j] = 0;
	}
	std::set<Player*, Scores::compare>::iterator i = sc->scores.begin();
	while ( i != sc->scores.end() ){
		Player* p = (*i);
		for (int j = 0; j < 7; j++){
			if (p->stats[j] > max[j])
				max[j] = p->stats[j];
		}
		if (p->getScore() > max[7])
			max[7] = p->getScore();
		i++;
	}

	///Generate all the text
	Text* r;
	r = new Text(ddc);
	r->printf("Game Summary");
	r->translate(0.3f, 0.01f, 0.0f);
	r->scale(0.09f, 0.09f, 1.0f);
	items.insert(r);

	r= new Text(ddc);
	r->printf("Chain  1's  2's  3's  4's  5's  6's  Total");
	r->scale(0.06f, 0.06f, 1.0f);
	r->translate(0.1f, 0.1f, 0.0f);
	items.insert(r);

	float yoff = 0.2f;
	float ystep = 0.1f;
	i = sc->scores.begin();
	while ( i != sc->scores.end() ){
		Player* p = (*i);
		float xoff = 0.01f;
		r = new Text(ddc);
		r->printf("%s", p->getName().c_str());
		r->translate(xoff, yoff, 0.0f);
		r->scale(0.065f, 0.07f, 1.0f);
		items.insert(r);
		xoff += 0.13f;
		for (int j = 0; j < 7; j++){
			r = new Text(ddc);
			r->printf("%i", p->stats[j]);
			r->translate(xoff, yoff, 0.0f);
			r->scale(0.055f, 0.06f, 1.0f);
			if (p->stats[j] == max[j] && max[j])
				r->color(1.0f,0.0f,0.0f);
			items.insert(r);
			xoff += 0.1f;
		}
			r= new Text(ddc);
			r->printf("%i", p->getScore());
			r->scale(0.055f, 0.06f, 1.0f);
			r->translate(xoff, yoff, 0.0f);
			if (p->getScore() == max[7] && max[7])
				r->color(1.0f,0.0f,0.0f);
			items.insert(r);
		
		i++;
		yoff += ystep;
	}

}

Summary::~Summary()
{
	std::set<Renderable*>::iterator i = items.begin();
	while ( i != items.end() ){
		delete(*i);
		i++;
	}
}

void Summary::render(Context* c){
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ); 
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	
	std::set<Renderable*>::iterator i = items.begin();
	while ( i != items.end() ){
		(*i)->render(c);
		i++;
	}

	c->m_mspWorld->Pop();
}