// ScreenShot.cpp: implementation of the ScreenShot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScreenShot.h"
#include "DXSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScreenShot::ScreenShot(DXSystem* dxs, int width, int height):
	Banner(dxs, "No File Needed", false)
{
	dxs->releaseTexture(pTex);
	pTex = dxs->screenShot(width, height);
}

ScreenShot::~ScreenShot()
{
	pTex->Release();
	pTex = NULL;
}
