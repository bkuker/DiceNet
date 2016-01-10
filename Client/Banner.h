// Banner.h: interface for the Banner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BANNER_H__E61FECA5_3024_465B_897F_6CC2532E5562__INCLUDED_)
#define AFX_BANNER_H__E61FECA5_3024_465B_897F_6CC2532E5562__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIElement.h"

class Banner : public UIElement  
{
public:
	Banner(DXSystem* dxs, std::string texFile, bool lit = 1);
	virtual ~Banner();
	virtual void draw();
	void setArea(float x1, float y1, float x2, float y2);

protected:
	void init();
	LPDIRECT3DTEXTURE8 pTex;
	static IDirect3DVertexBuffer8 *polys;
	static int refCount;
	D3DXMATRIX tm;
	int lit;
};

#endif // !defined(AFX_BANNER_H__E61FECA5_3024_465B_897F_6CC2532E5562__INCLUDED_)
