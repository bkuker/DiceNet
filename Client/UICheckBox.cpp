// UICheckBox.cpp: implementation of the UICheckBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UICheckBox.h"
#include "Text.h"
#include "Banner.h"
#include "DXSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UICheckBox::UICheckBox(DXSystem* dxs, std::string _text, bool startValue, std::string varName, std::string _command ):
value(startValue),
active(false),
UIContainer(dxs),
variable(varName),
command(_command)
{
	
	text = new Text(dxs);
	mark = new Text(dxs);
	text->printf("%s", _text.c_str());
	mark->printf(value?"X":" ");
	text->scale( (0.75f*3.0f)/(_text.length()+1), 1, 1);
	mark->scale(0.25f,1,1);
	mark->translate(0.86f,0,0);
	
	text->color(0.8f,0.8f,0.8f);
	mark->color(0.8f,0.8f,0.8f);
	
	add(text);
	add(mark);
	
}

UICheckBox::~UICheckBox()
{

}

void UICheckBox::mouseEvent(UIMouseButton b, UIMouseEvent e)
{
	if ( b == UI_LEFT ){
		if ( e == UI_DOWN ){
			active = true;
		} else if ( e == UI_UP && active) {
			value = !value;
			mark->printf(value?"X":" ");
			setVariable();
			if ( command != "" )
				execute();
		}
	}
}

void UICheckBox::getFocus( D3DXVECTOR3 )
{
	text->color(1,1,1);
}

void UICheckBox::looseFocus()
{
	text->color(0.8f,0.8f,0.8f);
	setVariable();
}

void UICheckBox::setVariable(){
	char tmp[128];
	strcpy(tmp, ("set " + variable + " " + std::string(value?"true":"false")).c_str() );
	dxs->console->processConsoleCmd( tmp );
}

void UICheckBox::execute(){
	char tmp[128];
	strcpy( tmp, command.c_str() );
	dxs->console->processConsoleCmd( tmp );
}