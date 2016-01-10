// Game.h: interface for the Game class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDClient_H__79C09CDE_C501_4281_8498_B379A4927EFD__INCLUDED_)
#define AFX_DDClient_H__79C09CDE_C501_4281_8498_B379A4927EFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DXSystem.h"
#include "Client.h"
#include "Renderable.h"
#include "Context.h"
#include "../packets.h"
#include "Player.h"
#include "Board.h"
class UIContainer;
class UIViewport;
class Board;
class Text;
class Summary;

class DDClient : public DXSystem, public Client
{
friend Summary;
public:
	DDClient(char* appTitle, HINSTANCE hInstance);
	virtual ~DDClient();
	virtual void onLostDevice();
	virtual void onResetDevice();
	virtual void drawScene();
	virtual void inputCB(BOOL* input);
	Player* player(int n);

//Network Related
public:
	void onDisconnect();
	void receive( PACKET*, int );

//Console stuff:
public:
	void setupConsole();
	void cmd_chat(int argc, char** argv);
	void cmd_connect(int argc, char** argv);
	void cmd_disconnect(int argc, char** argv);
	void cmd_name(int argc, char** argv);
	void cmd_model(int argc, char** argv);
	void cmd_menu(int argc, char** argv);
	void cmd_resume(int argc, char** argv);
	void cmd_modelview(int argc, char** argv);
	
	
	std::map<int, Player*> players;
//Private game related things
private:
	std::string name;
	std::string model;
	int playerNumber;
	
	CELL_DIR lastMove;
	Board* board;

	//Renderable *c,*b,*i, *n;
	Scores* scores;
	Context m_cHud, m_cWorld;
	D3DXMATRIX matWorld, matView, matWProj, matHudProj, matIdent, mt;

	Info* info;
	Summary* sum;
	Text *messageText, *levelText;
	int messageTime;
	UIContainer *gameUI, *menuUI, *summaryUI, *introUI;
	UIViewport* modelSelect;
};

#endif // !defined(AFX_Game_H__79C09CDE_C501_4281_8498_B379A4927EFD__INCLUDED_)
