// Board.h: interface for the Board class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Board_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_)
#define AFX_Board_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cell.h"
class DDServer;
struct CLIENT;

class Board
{
public:
	Board(DDServer* _dds, int size);
	virtual ~Board();
	
	void tellClient(CLIENT* c);
	int getSize();
	void reset();
	float filled();
	void fill(float d);
	void setFreq(float l){ newFreq1 = l; }; //Set the fill frequency in dice per second
	void tick(int milliseconds);
	Cell* cell(int x, int y);
	bool gameOver();


private:
	DDServer* dds;
	Cell** cells;
	int size;
	float newFreq1, newFreq2;
	int level;
};

#endif // !defined(AFX_Board_H__F443A027_73E4_4636_9D65_7A998AE79BF9__INCLUDED_)
