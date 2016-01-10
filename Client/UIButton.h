// UIButton.h: interface for the UIButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIBUTTON_H__BAC6C6C9_C844_41A4_BB97_6F33F7115928__INCLUDED_)
#define AFX_UIBUTTON_H__BAC6C6C9_C844_41A4_BB97_6F33F7115928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"
class Banner;
class Text;

class UIButton : public UIElement  
{
public:
	UIButton(DXSystem* dxs, std::string command, std::string text, D3DXVECTOR3 color = D3DXVECTOR3(.5,.5,.5));
	virtual ~UIButton();

public: //Events
	virtual void getFocus( D3DXVECTOR3 );
	virtual void looseFocus();
	virtual void mouseEvent(UIMouseButton, UIMouseEvent);
	virtual void render(Context*);
	virtual void execute();

private:

	Banner *image, *imageFocus, *imageActivate, *banner;
	Text* text;
	static Banner *DEFimage, *DEFimageFocus, *DEFimageActivate;
	std::string command;
};

#endif // !defined(AFX_UIBUTTON_H__BAC6C6C9_C844_41A4_BB97_6F33F7115928__INCLUDED_)
