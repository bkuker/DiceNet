// UIClickWrapper.cpp: implementation of the UIClickWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIClickWrapper.h"
#include "DXSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIClickWrapper::UIClickWrapper(DXSystem* dxs, Renderable* _content, std::string _cmd):
	UIElement(dxs),
	content(_content),
	command(_cmd)
{

}

UIClickWrapper::~UIClickWrapper()
{

}


void UIClickWrapper::mouseEvent(UIMouseButton, UIMouseEvent)
{
	execute();
}

void UIClickWrapper::keyEvent(WPARAM k, UI_KEY_EVENT e)
{
	execute();
}

void UIClickWrapper::render(Context* c){
	content->render(c);
}

void UIClickWrapper::execute(){
	char tmp[128];
	strcpy( tmp, command.c_str() );
	dxs->console->processConsoleCmd( tmp );
}