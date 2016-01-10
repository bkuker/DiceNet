// UITextInput.cpp: implementation of the UITextInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UITextInput.h"
#include "Text.h"
#include "Banner.h"
#include "Context.h"
#include "DXSystem.h"
#include "UIButton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UITextInput::UITextInput(DXSystem* dxs, int _width, std::string var, UIButton* button, std::string txt, std::string _command ):
	UIElement(dxs),
	text(new Text(dxs)),
	banner(new Banner(dxs, "textures/input.bmp")),
	width(_width),
	variable(var),
	onReturn(button),
	command(_command)
{
	value[0] = 0;
	strcpy( value, txt.c_str() );
	text->scale( 3.0f/(width), 1,1);
	text->printf("%s", value);
	text->color(0.8f,0.8f,0.8f);
	setVariable();
}

UITextInput::~UITextInput()
{
	
}

void UITextInput::keyEvent(WPARAM w, UI_KEY_EVENT e)
{
	if (w == VK_RETURN && onReturn != NULL){
		if ( e == UI_KEY_DOWN ){
			onReturn->mouseEvent(UI_LEFT, UI_DOWN);
		} else if ( e == UI_KEY_UP ){
			setVariable();
			onReturn->mouseEvent(UI_LEFT, UI_UP);
		}
	}
}

void UITextInput::character(char c)
{
		int	len = strlen(value);
		switch(c){
		case '\b':
			if(len)
				value[len-1] = 0;
			break;
		case '\r':
		case '\n':
			if (command != "" ){
				setVariable();
				execute();
			}
		case '\t':
		case 27:
			break;
		default:
			if(len < UI_INPUT_LINE_LENGTH){
				value[len] = c;
				value[len+1] = 0;
			}
		}
		char tmp = value[width];
		value[width] = 0;
		text->printf("%s_", value);
		value[width] = tmp;
}
 
void UITextInput::getFocus( D3DXVECTOR3 )
{
	text->printf("%s_", value);
	text->color(1,1,1);
}

void UITextInput::looseFocus()
{
	text->printf("%s", value);
	text->color(0.8f,0.8f,0.8f);
	setVariable();
	if (command != "" ){
		
		execute();
	}
	
}

void UITextInput::render(Context *c){
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	draw();
	banner->render(c);
	text->render(c);
	c->m_mspWorld->Pop();
}

void UITextInput::setVariable(){
	char tmp[128];
	strcpy(tmp, ("set " + variable + " " + std::string(value)).c_str() );
	dxs->console->processConsoleCmd( tmp );
}

void UITextInput::execute(){
	char tmp[128];
	strcpy( tmp, command.c_str() );
	dxs->console->processConsoleCmd( tmp );
}