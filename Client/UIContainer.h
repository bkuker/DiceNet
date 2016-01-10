// UIContainer.h: interface for the UIContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UICONTAINER_H__06C58BA3_7D80_44C0_B369_9593278125BA__INCLUDED_)
#define AFX_UICONTAINER_H__06C58BA3_7D80_44C0_B369_9593278125BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"

class UIContainer : public UIElement  
{
public:
	UIContainer(DXSystem* dxs);
	virtual ~UIContainer();

	virtual void add(UIElement*);
	virtual void remove(UIElement*);
	virtual void setBG(UIElement*);
	virtual UIElement* getBG();
	virtual void render(Context* c);
	virtual void draw(){};

public: //Events
	virtual void getFocus( D3DXVECTOR3 );
	virtual void looseFocus();
	virtual void mouseEvent(UIMouseButton, UIMouseEvent);
	virtual void keyEvent(WPARAM, UI_KEY_EVENT);
	virtual void character(char);

private:

	std::set<UIElement*> children;

	UIElement* focusE;
	UIElement* bg;

};

#endif // !defined(AFX_UICONTAINER_H__06C58BA3_7D80_44C0_B369_9593278125BA__INCLUDED_)
