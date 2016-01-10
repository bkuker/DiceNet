// UIContainer.cpp: implementation of the UIContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIContainer.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIContainer::UIContainer(DXSystem* dxs):
	UIElement(dxs),
	focusE(NULL),
	bg(NULL)
{

}

UIContainer::~UIContainer()
{

}

void UIContainer::add(UIElement* e)
{
	children.insert(e);
}

void UIContainer::remove(UIElement* e)
{
	std::set<UIElement*>::iterator i = children.begin();
	for(;i != children.end(); i++ ){
		if ((*i) == e){
			children.erase(i);
			return;
		}
	}
}

void UIContainer::setBG(UIElement* _bg)
{
	bg = _bg;
}

UIElement* UIContainer::getBG()
{
	return bg;
}


void UIContainer::render(Context *c){
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	if ( bg != NULL )
		bg->render(c);
	draw();
	std::set<UIElement*>::iterator i = children.begin();
	for(;i != children.end(); i++ ){
		(*i)->render(c);
	}
	c->m_mspWorld->Pop();
}

void UIContainer::getFocus( D3DXVECTOR3 pos )
{
	D3DXMATRIX m;
	D3DXMatrixInverse( &m, NULL, transform() );
	D3DXVec3TransformCoord( &pos, &pos, &m );
	std::set<UIElement*>::iterator i = children.begin();
	for(;i != children.end(); i++ ){
		UIElement* e = (*i);
		if ( e->checkPos(pos) ){
			if ( e != focusE ){
				if (focusE != NULL )
					focusE->looseFocus();
				e->getFocus(pos);
				focusE = e;
			}
		} else if ( e == focusE ) {
			e->looseFocus();
			focusE = NULL;
		}
	}
}

void UIContainer::looseFocus()
{
	
}

void UIContainer::mouseEvent(UIMouseButton b, UIMouseEvent e)
{
	if ( focusE != NULL ){
		focusE->mouseEvent(b, e);
	}
}

void UIContainer::keyEvent(WPARAM k, UI_KEY_EVENT e)
{
	if ( focusE != NULL ){
		focusE->keyEvent(k, e);
	}
}

void UIContainer::character(char c)
{
	if ( focusE != NULL ){
		focusE->character(c);
	}
}
