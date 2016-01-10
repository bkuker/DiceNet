// Info.h: interface for the Info class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFO_H__4A5B4B6A_5CBD_49FA_AC48_AD3B14477238__INCLUDED_)
#define AFX_INFO_H__4A5B4B6A_5CBD_49FA_AC48_AD3B14477238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"
class Banner;
class Player;

class Info : public UIElement 
{
public:
	Info(DXSystem* dxs, Player* player);
	void render(Context* c);
	virtual ~Info();

protected:
	Banner* banner;
	Player* m_Player;
	D3DXMATRIX s_mDieView;
};

#endif // !defined(AFX_INFO_H__4A5B4B6A_5CBD_49FA_AC48_AD3B14477238__INCLUDED_)
