// ScreenShot.h: interface for the ScreenShot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENSHOT_H__062155E1_0107_4F2D_9624_1AE91F8D8388__INCLUDED_)
#define AFX_SCREENSHOT_H__062155E1_0107_4F2D_9624_1AE91F8D8388__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Banner.h"

class ScreenShot : public Banner  
{
public:
	ScreenShot(DXSystem* dxs, int width, int height);
	virtual ~ScreenShot();

};

#endif // !defined(AFX_SCREENSHOT_H__062155E1_0107_4F2D_9624_1AE91F8D8388__INCLUDED_)
