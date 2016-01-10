// Text.h: interface for the Text class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXT_H__FB5600CE_F961_484D_BB03_788FCA4BF106__INCLUDED_)
#define AFX_TEXT_H__FB5600CE_F961_484D_BB03_788FCA4BF106__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"
#include "Banner.h"
#define MAX_TEXT 255

enum JUSTIFICATION {LEFT, RIGHT, CENTER};

class Text : public UIElement  
{
public:
	Text(DXSystem* dxs);
	virtual ~Text();
	void render(Context* c);
	void draw(){};
	void printf(const char *, ...);
	void color(float r, float g, float b);
	void vprintf(const char *, va_list argptr);
	void justify(JUSTIFICATION _j){ j = _j; };
	void onResetDevice();

protected:
	char text[MAX_TEXT];
	static D3DXMATRIX s_mR1, s_mL1;
	JUSTIFICATION j;
	static Banner* b;
	static int refCount;
	D3DMATERIAL8 m_mtrl;
	D3DXVECTOR3 m_v3Color;
};

#endif // !defined(AFX_TEXT_H__FB5600CE_F961_484D_BB03_788FCA4BF106__INCLUDED_)
