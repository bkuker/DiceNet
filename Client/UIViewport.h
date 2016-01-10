// UIViewport.h: interface for the UIViewport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIVIEWPORT_H__467881E5_B0AF_424E_BF63_300A2D3ACAB0__INCLUDED_)
#define AFX_UIVIEWPORT_H__467881E5_B0AF_424E_BF63_300A2D3ACAB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"

class UIViewport : public UIElement  
{
public:
	UIViewport(DXSystem*);
	UIViewport(DXSystem* dxs, Renderable* o);
	virtual ~UIViewport();
	void render(Context* c);
	void setObject(Renderable* o);
	Renderable* getObject();
private:
	Renderable* m_pObject;
	D3DXMATRIX m_mView;

};

#endif // !defined(AFX_UIVIEWPORT_H__467881E5_B0AF_424E_BF63_300A2D3ACAB0__INCLUDED_)
