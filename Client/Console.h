// Console.h: interface for the Console class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_)
#define AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class Console;

#include "Renderable.h"
#include "DXSystem.h"

#include "../Sconsole.h"

//console defines
#define CONSOLE_WIDTH	75
#define CONSOLE_LINES	100
#define CONSOLE_HIDDEN 0
#define CONSOLE_HALF -1
#define CONSOLE_FULL -2

struct CONSOLE_LINE{
	char	text[CONSOLE_WIDTH+1];
	struct	CONSOLE_LINE *next, *prev;
};

// this is for client callback setups

class Console_helper
{
public:

	Console_helper(Console* _c){ c = _c; };
	
	void clearConsole(int argc = 0, char** argv = 0);
	void help(int argc = 0, char** argv = 0);
	void setConsoleStatus(int argc = 0, char** argv = 0);

	Console* c;
};


class Console : public Renderable, public Sconsole 
{

friend DXSystem;
friend Sconsole_helper;
friend Console_helper;

public:
	Console(DXSystem* dxs);
	virtual ~Console();

//Console functions
public:
	

	void setConsolePosition(int pos);
	
	void cprintf(const char *, ...);
	void vcprintf(const char *, va_list argptr);
	void lockConsole(BOOL);
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void draw();
	void setTexture( LPDIRECT3DTEXTURE8 tex );
	bool visible(){ return desiredPosition != 0; };

//Key input functions
//These are public because of the windows event loop
//Do not rely on them.
public:
	void keyEvent(WPARAM dir);
	void charInput(WPARAM input);

	void help(int argc = 0, char** argv = 0);

//Private functions
protected:
	void consoleOutput(std::string message);
	void setConsoleStatus( int argc, char** argv);	
	void clearConsole(int argc = 0, char** argv = 0);	

//Console vars
private:
	
	char	inputLine[CONSOLE_WIDTH * 2]; //Since variable prompts can get us in trouble give it
											//eh, say 2x the space. ram = cheap eh?
	char prompt[25];
	BOOL showInput, cLock;
	CONSOLE_LINE *consoleHead, *consoleFoot, *consoleCurrent;
	int	numLines, position, desiredPosition;

	D3DXMATRIX				matConsView;
	IDirect3DVertexBuffer8	*conpolys;
	LPDIRECT3DTEXTURE8		contex;
	LPD3DXFONT				pdxfDefFont;

	Console_helper* cho;


};

#endif // !defined(AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_)
