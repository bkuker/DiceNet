// UITextInput.h: interface for the UITextInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UITEXTINPUT_H__7A18553D_D44C_44AF_A641_745C4013442E__INCLUDED_)
#define AFX_UITEXTINPUT_H__7A18553D_D44C_44AF_A641_745C4013442E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UI_INPUT_LINE_LENGTH 128
#include "UIElement.h"
class Text;
class Banner;
class UIButton;

class UITextInput : public UIElement  
{
public:
	UITextInput(DXSystem* dxs, int width, std::string var, UIButton*, std::string txt = "", std::string command = "" );
	virtual ~UITextInput();
	void character(char);
	void render(Context*);
	virtual void getFocus( D3DXVECTOR3 );
	virtual void looseFocus();
	virtual void keyEvent(WPARAM, UI_KEY_EVENT);

private:
	void execute();
	void setVariable();
	Text* text;
	Banner* banner;
	char value[UI_INPUT_LINE_LENGTH];
	int width;
	std::string variable, command;
	UIButton* onReturn;

};

#endif // !defined(AFX_UITEXTINPUT_H__7A18553D_D44C_44AF_A641_745C4013442E__INCLUDED_)
