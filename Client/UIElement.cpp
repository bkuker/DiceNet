// UIElement.cpp: implementation of the UIElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIElement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIElement::UIElement(DXSystem* dxs):
	Renderable(dxs)
{
	
}

UIElement::~UIElement()
{
	
}

bool UIElement::checkPos( D3DXVECTOR3 _pos )
{
	D3DXMATRIX m;
	D3DXVECTOR3 pos;
	D3DXMatrixInverse( &m, NULL, transform() );
	D3DXVec3TransformCoord( &pos, &_pos, &m );
	if ( pos.x <= 1 && pos.x >= 0 && pos.y <= 1 && pos.y >= 0 ){
		return true;
	}
	return false;
}

void UIElement::getFocus( D3DXVECTOR3 )
{
	
}

void UIElement::looseFocus()
{
	
}

void UIElement::mouseEvent(UIMouseButton, UIMouseEvent)
{
	
}

void UIElement::keyEvent(WPARAM k, UI_KEY_EVENT e)
{
	
}

void UIElement::character(char)
{
	
}
