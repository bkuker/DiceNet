// UIElement.h: interface for the UIElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIELEMENT_H__385648A6_4E46_4E4B_BF44_E13F4B18C5D4__INCLUDED_)
#define AFX_UIELEMENT_H__385648A6_4E46_4E4B_BF44_E13F4B18C5D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"


enum UIMouseButton { UI_LEFT, UI_RIGHT, UI_MIDDLE, UI_WHEEL };
enum UIMouseEvent { UI_UP, UI_DOWN };
enum UI_KEY_EVENT { UI_KEY_UP, UI_KEY_DOWN };

class UIElement  :	public Renderable
{
public:
	UIElement(DXSystem* dxs);
	virtual ~UIElement();

	virtual bool checkPos( D3DXVECTOR3 );	
	virtual void draw(){};

public: //Events
	virtual void getFocus( D3DXVECTOR3 );
	virtual void looseFocus();
	virtual void mouseEvent(UIMouseButton, UIMouseEvent);
	virtual void keyEvent(WPARAM, UI_KEY_EVENT);
	virtual void character(char);


private:

};

#endif // !defined(AFX_UIELEMENT_H__385648A6_4E46_4E4B_BF44_E13F4B18C5D4__INCLUDED_)
