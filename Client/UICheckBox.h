// UICheckBox.h: interface for the UICheckBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UICHECKBOX_H__398C0A91_A28D_4951_A2C6_B6C5A68AF328__INCLUDED_)
#define AFX_UICHECKBOX_H__398C0A91_A28D_4951_A2C6_B6C5A68AF328__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIContainer.h"
class Text;
class Banner;

class UICheckBox : public UIContainer  
{
public:
	UICheckBox(DXSystem*, std::string text, bool startValue, std::string varName, std::string command = "" );
	virtual ~UICheckBox();

	virtual void mouseEvent(UIMouseButton, UIMouseEvent);
	virtual void getFocus( D3DXVECTOR3 );
	virtual void looseFocus();

private:
	Text *text, *mark;
	Banner *box;
	std::string variable, command;
	bool value, active;
	void execute();
	void setVariable();

};

#endif // !defined(AFX_UICHECKBOX_H__398C0A91_A28D_4951_A2C6_B6C5A68AF328__INCLUDED_)
