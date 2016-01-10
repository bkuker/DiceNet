// UIButton.cpp: implementation of the UIButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"
#include "Banner.h"
#include "Text.h"
#include "Context.h"
#include "Console.h"


Banner *UIButton::DEFimage = NULL, *UIButton::DEFimageFocus = NULL, *UIButton::DEFimageActivate = NULL;
#define DEFIMAGE "textures/button.dds"
#define DEFIMAGEACTIVATE "textures/buttonactive.dds"
#define DEFIMAGEFOCUS "textures/buttonfocus.dds"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIButton::UIButton(DXSystem* dxs, std::string _command, std::string _text, D3DXVECTOR3 color):
	UIElement(dxs),
	command(_command)
{
		if (DEFimage == NULL){
			DEFimage = new Banner(dxs, DEFIMAGE);
			DEFimageFocus = new Banner(dxs, DEFIMAGEFOCUS);
			DEFimageActivate = new Banner(dxs, DEFIMAGEACTIVATE);
		}

		image = DEFimage;
		imageFocus = DEFimageFocus;
		imageActivate = DEFimageActivate;

		text = new Text(dxs);
		text->printf(_text.c_str());
		text->scale( 3.0f/(_text.length()+1), 1, 1);

		banner = image;
			
}

UIButton::~UIButton()
{

}

void UIButton::render(Context *c){
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	draw();
	banner->render(c);
	text->render(c);
	c->m_mspWorld->Pop();
}

void UIButton::getFocus( D3DXVECTOR3 pos )
{
	banner = imageFocus;
}

void UIButton::looseFocus()
{
	banner = image;
}

void UIButton::mouseEvent(UIMouseButton b, UIMouseEvent e)
{
	if ( b == UI_LEFT ){
		if ( e == UI_DOWN ){
			banner = imageActivate;
		} else if ( e == UI_UP && banner == imageActivate) {
			execute();
			banner = imageFocus;
		}
	}
}

void UIButton::execute(){
	char tmp[128];
	strcpy( tmp, command.c_str() );
	dxs->console->processConsoleCmd( tmp );
}