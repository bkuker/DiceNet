// Summary.h: interface for the Summary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUMMARY_H__7726C1E3_A13B_42C9_A86F_5E3B4F475F1A__INCLUDED_)
#define AFX_SUMMARY_H__7726C1E3_A13B_42C9_A86F_5E3B4F475F1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"
#include "DDClient.h"
class Scores;

class Summary : public Renderable 
{
public:
	Summary(DDClient* _ddc, Scores* sc);
	virtual ~Summary();
	void draw(){};
	void render(Context* c);

private:
	std::set<Renderable*> items;
};

#endif // !defined(AFX_SUMMARY_H__7726C1E3_A13B_42C9_A86F_5E3B4F475F1A__INCLUDED_)
