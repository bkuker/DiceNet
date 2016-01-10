// UIClickWrapper.h: interface for the UIClickWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UICLICKWRAPPER_H__3E378952_0E55_410B_9C4D_5897936BE89B__INCLUDED_)
#define AFX_UICLICKWRAPPER_H__3E378952_0E55_410B_9C4D_5897936BE89B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"

class UIClickWrapper : public UIElement  
{
public:
	UIClickWrapper(DXSystem* dxs, Renderable* content, std::string cmd);
	virtual ~UIClickWrapper();
	void render(Context* c);

	virtual void mouseEvent(UIMouseButton, UIMouseEvent);
	virtual void keyEvent(WPARAM, UI_KEY_EVENT);

private:
	void execute();
	Renderable* content;
	std::string command;
};

#endif // !defined(AFX_UICLICKWRAPPER_H__3E378952_0E55_410B_9C4D_5897936BE89B__INCLUDED_)
