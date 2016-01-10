// Board.h: interface for the Board class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Board_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_)
#define AFX_Board_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Renderable.h"
#include "Animated.h"
#include "Moveable.h"
class Cell;
class Context;
class DDClient;


class Board : 
	public Animated, public Renderable
{
public:
	Board(DDClient* dxs, int size, std::string fTex);
	virtual ~Board();

	void tick(int milliseconds);
	Cell* cell(int x, int y);

	void render(Context* c);
	void draw();


private:
	void init(int size);
	LPDIRECT3DTEXTURE8 pTex;
	IDirect3DVertexBuffer8	*m_pvPoints;
	Cell** cells;
	int size;
	float newFreq;
};

#endif // !defined(AFX_Board_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_)
