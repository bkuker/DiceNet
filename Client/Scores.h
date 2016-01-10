// Scores.h: interface for the Scores class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCORES_H__97D2546A_589A_4EE7_B5A2_BE5607EE157D__INCLUDED_)
#define AFX_SCORES_H__97D2546A_589A_4EE7_B5A2_BE5607EE157D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"
#include "DDClient.h"
#include <set>
class Summary;



class Scores : public UIElement  
{
friend Summary;
public:
	Scores(DDClient* _ddc);
	void render(Context* c);
	void draw(){};
	void score_change();
	virtual ~Scores();
	struct compare {
		bool operator() (const Player *a, const Player *b) const;
	};
private:
	DDClient* ddc;
	static D3DXMATRIX s_mD1;
	std::set<Player*, compare> scores;

};

#endif // !defined(AFX_SCORES_H__97D2546A_589A_4EE7_B5A2_BE5607EE157D__INCLUDED_)
