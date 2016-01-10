// UIViewport.cpp: implementation of the UIViewport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIViewport.h"
#include "Context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIViewport::UIViewport(DXSystem* dxs):
	UIElement(dxs),
	m_pObject(NULL)
{
	D3DXMATRIX m;

	D3DXMatrixLookAtLH(&m_mView, 
		&D3DXVECTOR3( 4,3,-4),
		&D3DXVECTOR3( 0,1,0),
		&D3DXVECTOR3( 0,1,0));
/*	D3DXMatrixScaling( &s_mDiscard, 1.2f, 1.2f, 1 );
	D3DXMatrixMultiply( &m_mView, &m_mView, &m );*/

	D3DXMatrixPerspectiveFovLH( &m, -D3DX_PI/4, -1.0f, 0.1f, 1000.0f );
	D3DXMatrixMultiply( &m_mView, &m_mView, &m );
/*
	D3DXMatrixTranslation( &m, 0.5f, 0.5f, 0 );
	D3DXMatrixMultiply( &m_mView, &m_mView, &m );*/
}

UIViewport::UIViewport(DXSystem* dxs, Renderable* o):
	UIElement(dxs),
	m_pObject(0)
{
	
}

void UIViewport::setObject(Renderable* o)
{
	m_pObject = o;
}

Renderable* UIViewport::getObject(){
	return m_pObject;
}


UIViewport::~UIViewport()
{

}

void UIViewport::render(Context* c)
{
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	if ( m_pObject != NULL )
	{
		c->m_mspWorld->MultMatrixLocal( &m_mView );
		m_pObject->render(c);
	} 
	c->m_mspWorld->Pop();
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
}