// DDClient.cpp: implementation of the DDClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DDClient.h"
#include "Board.h"
#include "Cell.h"
#include "Scores.h"
#include "../ConfigFile.h"
#include "inputs.h"
#include "Text.h"
#include "Info.h"
#include "Summary.h"
#include "UIButton.h"
#include "UIContainer.h"
#include "UITextInput.h"
#include "UIClickWrapper.h"
#include "UICheckBox.h"
#include "UIViewport.h"
#include "Banner.h"
#include "Screenshot.h"
#include "cutscene.h"

#include "Die.h"

#include <time.h>

void inputParamCB(int &numActions, GUID &DDClientguid, DWORD &genre, DIACTION* &actions, TCHAR name[MAX_PATH]);

void pprint(PACKET* p, Console* c){
	for (int i = 0; i < p->size; i++)
		c->cprintf("%x\n", (byte)((byte*)p)[i]);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DDClient::DDClient(char* appTitle, HINSTANCE hInstance)
:DXSystem(appTitle, hInstance, &inputParamCB, new ConfigFile("dd.ini"), true),
messageText(NULL),
levelText(NULL),
info(NULL),
sum(NULL)
{

	introUI = new UIContainer(this);
	gameUI = new UIContainer(this);
	menuUI = new UIContainer(this);
	summaryUI = new UIContainer(this);
	
	setupConsole();

	D3DXMatrixIdentity( &matIdent );

	//Build world context
	D3DXMatrixLookAtLH(&matView, 
		&D3DXVECTOR3( 8,8,-8),
		&D3DXVECTOR3( 0,-1,0),
		&D3DXVECTOR3( 0,1,0));
	D3DXMatrixPerspectiveFovLH( &matWProj, D3DX_PI/4, 4.0f/3.0f, 0.1f, 1000.0f );
	m_cWorld.SetTransform( D3DTS_WORLD, &matIdent );
	m_cWorld.SetTransform( D3DTS_VIEW, &matView);//m_Camera.transform() );
	m_cWorld.SetTransform( D3DTS_PROJECTION, &matWProj );

	//build hud context
	D3DXMatrixOrthoOffCenterLH(&matHudProj, 0, 1, 1, 0, 0, 1000);
	m_cHud.SetTransform( D3DTS_WORLD, &matIdent );
	m_cHud.SetTransform( D3DTS_VIEW, &matIdent );
	m_cHud.SetTransform( D3DTS_PROJECTION, &matHudProj );
 
	//Set up the light
	D3DLIGHT8 light;
	ZeroMemory( &light, sizeof(D3DLIGHT8) );
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Direction = D3DXVECTOR3(-1,-4,1);
	pd3dDevice->SetLight( 0, &light );
	pd3dDevice->LightEnable( 0, TRUE);

	//Mute sound so I dont go 'naners
	//dxs->mute(true);

	//Seed the random number generator
	srand( (unsigned)time( NULL ) );

	//setup client stuff
	cf->pushs("game");
	name = cf->getStr( "name" );
	model = cf->getStr( "model" );
/*	try{
		if ( cf->isKey( "server" ) ){
			if ( cf->isKey( "port" ) ){
				Client::connect( cf->getStr("server").c_str(), cf->getInt("port") );
			} else {
				Client::connect( cf->getStr("server").c_str(), DEFAULT_PORT );
			}
			send( (PACKET*)&P_PLAYERJOIN(0, name.c_str(), model.c_str() ) );
		}
	} catch ( const char* e ) {
		console->cprintf("Error connecting: %s", e);
		setUI(menuUI);
	}*/
	cf->pops();
	lastMove = NONE;
	board = NULL;

	scores = new Scores(this);
	scores->scale(0.06f, 0.06f, 1.0f);
	scores->translate(0.0f, 0.02f, 0.0f);

	messageText = new Text(this);
	messageText->scale(0.07f, 0.07f, 1.0f);
	messageText->translate(0.05f,0.9f,0.0f);
	messageText->printf("Welcome to Daemon Dice!");
	messageTime = 10000;

	levelText = new Text(this);
	levelText->scale(0.07f, 0.07f, 1.0f);
	levelText->translate(0.78f,0.05f,0.0f);

	gameUI->add(scores);
	gameUI->add(messageText);
	gameUI->add(levelText);


	//UI test
	cf->pushs("game");

	Text* stext = new Text(this);
	stext->printf("Server Name");
	stext->scale(0.06f,0.05f,1);
	stext->translate(0.03f,0.05f,0);
	menuUI->add(stext);

	UIButton* b = new UIButton(this, "connect $menuServer", "Connect");
	b->scale(0.15f,0.06f,1);
	b->translate(0.3f,0.17f,0);
	menuUI->add(b);

	b = new UIButton(this, "disconnect", "Disconnect");
	b->scale(0.15f,0.06f,1);
	b->translate(0.03f,0.17f,0);
	menuUI->add(b);

	UITextInput* t = new UITextInput(this, 20, "menuServer", b, cf->isKey("server")?cf->getStr("server"):"localhost", "");
	t->scale(.4,0.06f,1);
	t->translate(0.03f,0.1f,0);
	menuUI->add(t);

	b = new UIButton(this, "resume", "Resume Game");
	b->scale(.2,.1,1);
	b->translate(.4,0.8f,0);
	menuUI->add(b);
	b = new UIButton(this, "quit", "Quit");
	b->scale(.2,.1,1);
	b->translate(0.7f,0.8f,0);
	menuUI->add(b);


	stext = new Text(this);
	stext->printf("Player Name");
	stext->scale(0.06f,0.05f,1);
	stext->translate(0.4f,0.55f,0);
	menuUI->add(stext);

	t = new UITextInput(this, 20, "menuName", NULL, cf->isKey("name")?cf->getStr("name"):"Player", "name $menuName");
	t->scale(.4,0.06f,1);
	t->translate(0.4f,.6,0);
	menuUI->add(t);

	b = new UIButton(this, "modelview prev", "<");
	b->scale(0.05f,0.05f,1);
	b->translate(0.03f,0.77f,0);
	menuUI->add(b);

	b = new UIButton(this, "modelview set", "Select");
	b->scale(0.15f,0.05f,1);
	b->translate(0.085f,0.77f,0);
	menuUI->add(b);

	b = new UIButton(this, "modelview next", ">");
	b->scale(0.05f,0.05f,1);
	b->translate(0.23f,0.77f,0);
	menuUI->add(b);

	modelSelect = new UIViewport( this );
	modelSelect->scale(0.2f,0.3f,1);
	modelSelect->translate(0.15f,0.4f,0.0f);
	menuUI->add(modelSelect);
	cmd_modelview(0, NULL);

	cf->pushs("video");
	UIElement* u = new UICheckBox(this, "Fullscreen", isFullScreen(), "menuFS", "fullscreen $menuFS");
	u->scale(0.2f,0.07f,1.0f);
	u->translate(0.03f, 0.82f, 0);
	menuUI->add(u);
	cf->pops();

	u = new UICheckBox(this, "Mute", false, "menuMute", "mute");
	u->scale(0.2f,0.07f,1.0f);
	u->translate(0.03f, 0.87f, 0);
	menuUI->add(u);


	cf->pops();


	Banner* d = new Banner(this, "textures/bwdice.dds");
	d->scale(0.4f,0.4f,0.4f);
	d->translate(0.5f,0.1f,0);
	menuUI->add(d);

	introUI->add(new UIClickWrapper(this, new Banner(this, "textures/opening.bmp", 0), "menu"));
	setUI(introUI);

	//PlayCutscene("intro2.avi", hInstance, hWindow);
}

DDClient::~DDClient()
{
//	delete c;
//	delete b;
//	delete n;
//	delete i;
	std::map<int, Player*>::iterator it = players.begin();
	while (it != players.end() ){
		delete (it->second);
		it++;
	}
	safeDelete(sum);
	safeDelete(console);
	safeDelete(messageText);
	safeDelete(levelText);
	safeDelete(info);
	safeDelete(board);
}


void DDClient::onDisconnect(){
	console->cprintf("You have been disconnected from %s:%d", rhost.c_str(), rport);
	messageText->printf("Disconnected from %s:%d", rhost.c_str(), rport);
	messageTime = 1000;
	levelText->printf("");
	gameUI->remove(info);
	safeDelete(info);
	setUI(menuUI);
	std::map<int, Player*>::iterator it = players.begin();
	while (it != players.end() ){
		delete (it->second);
		it++;
	}
	players.clear();
	safeDelete(board);
}

void DDClient::receive( PACKET* p, int l){
	std::map<int, Player*>::iterator it;
	int i;
	P_PLAYERJOIN* pj;
	P_PLAYERNAME* pn;
	P_PLAYERMODEL* pm;
	P_PLAYERCELL* pc;
	P_PLAYERDEST* pd;
	P_CELLFILL*	cf;
	P_CELLSTEP* cstep;
	P_CELLSTATE* cstate;
	P_PLAYERWARP* pw;
	P_MSG* pmsg;
	P_PLAYERSTAT* pstat;
	pprint(p,console);
	switch (p->type){
		case T_CHAT:
			if ( ((P_CHAT*)p)->player == 0 ){
				console->cprintf("Server: %s", ((P_CHAT*)p)->str);
			} else {
				console->cprintf("%s: %s", players[((P_CHAT*)p)->player]->getName().c_str(), ((P_CHAT*)p)->str);
			}
			break;
		case T_MSG:
			pmsg = (P_MSG*)p;
			pmsg->p1 = ntohl(pmsg->p1);
			pmsg->p2 = ntohl(pmsg->p2);
			switch (pmsg->type){
				case MSG_LEVEL:
					levelText->printf("Level %i", pmsg->p1);
					break;
				case MSG_CHAIN:
					messageText->printf("Chain %i by %s", pmsg->p1, player(pmsg->p2)->getName().c_str());
					messageTime = 800;
					break;
				case MSG_GAMEOVER:
					messageText->printf("Game over.");
					messageTime = 100;
					levelText->printf("");
					sum = new Summary(this, scores);
					gameUI->remove(info);
					safeDelete(info);
					safeDelete(board);
					it = players.begin();
					while ( it != players.end() ){
						(it->second)->reset();
						it++;
					}
					break;
				case MSG_TTS:
					if (pmsg->p1 > 0)
						messageText->printf("Game starts in %i second%s...", pmsg->p1, pmsg->p1==1?"":"s");
					else
						messageText->printf("Game start!");
					messageTime = pmsg->p1<5?5:11;
					break;
			}
			break;
		case T_YOURPLAYER:
			playerNumber = ((P_YOURPLAYER*)p)->player;
			console->cprintf("You've joined the game.");
			messageText->printf("You have joined!");
			messageTime = 500;
			gameUI->remove(info);
			safeDelete(info);
			scores->score_change();
			break;
		case T_PLAYERJOIN:
			pj = (P_PLAYERJOIN*)p;
			it = players.find(pj->num);
			if ( it != players.end() )
				delete (Player*)(it->second);//delete the old one
			players[pj->num] = new Player(this, pj->num, pj->name, pj->model);
			console->cprintf("%s has joined the game.", pj->name);
			scores->score_change();
			break;
		case T_PLAYERNAME:
			pn = (P_PLAYERNAME*)p;
			it = players.find(pn->player);
			if ( it != players.end() ){
				console->cprintf("%s changed name to %s", (it->second)->getName().c_str(), pn->name);
				(it->second)->setName(pn->name);
			}
			break;
		case T_PLAYERMODEL:
			pm = (P_PLAYERMODEL*)p;
			it = players.find(pm->player);
				if ( it != players.end() )
					(it->second)->setModel(pm->model);
			break;
		case T_PLAYERDISCONNECT:
			it = players.find(((P_PLAYERDISCONNECT*)p)->player);
			if ( it != players.end() ){
					console->cprintf("%s has left the game.", (it->second)->getName().c_str());
					delete (it->second);
					players.erase(it);
			}
			scores->score_change();
			break;
		case T_REJECT:
			console->cprintf("You have been rejected. Reason: %s", ((P_REJECT*)p)->reason);
			disconnect();
			break;
		case T_BOARD:
			safeDelete(board);
			board = new Board(this, ((P_BOARD*)p)->size, "textures/board.dds");
			gameUI->remove(info);
			safeDelete(info);
			it = players.begin();
			while ( it != players.end() ){
				(it->second)->reset();
				it++;
			}
			if (playerNumber != 0){
				info = new Info(this, player(playerNumber) );
				info->translate(0.6f, 0.02f, 0.0f);
				info->scale(0.15f, 0.15f, 0.15f);
				gameUI->add(info);
			}
			setUI(gameUI);
			console->bind(VK_ESCAPE, "menu");
			safeDelete(sum);
			break;
		case T_PLAYERSCORE:
			it = players.find(((P_PLAYERSCORE*)p)->player);
			if ( it != players.end() ) 
				(it->second)->setScore(ntohl(((P_PLAYERSCORE*)p)->score));
			scores->score_change();
			break;
		case T_PLAYERCELL:
			pc = (P_PLAYERCELL*)p;
			it = players.find(pc->player);
			if ( it != players.end() )
				(it->second)->setCell(board->cell(pc->cellx,pc->celly));
			break;
		case T_PLAYERWARP:
			pw = (P_PLAYERWARP*)p;
			it = players.find(pw->player);
			if ( it != players.end() )
				(it->second)->setCellWarp(board->cell(pw->cellx,pw->celly));
			break;
		case T_PLAYERDEST:
			pd = (P_PLAYERDEST*)p;
			it = players.find(pd->player);
			if ( it != players.end() )
				(it->second)->setDest(board->cell(pd->cellx,pd->celly), (CELL_DIR)pd->edge, pd->up != 0);
			break;
		case T_PLAYERSTAT:
			pstat = (P_PLAYERSTAT*)p;
			for (i = 0; i < 7; i++)
				pstat->stats[i] = ntohl(pstat->stats[i]);
			player(pstat->player)->setStats(pstat->stats);
			break;
		case T_CELLFILL:
			cf = (P_CELLFILL*)p;
			if (board != NULL)
				board->cell(cf->cx,cf->cy)->fill(cf->up, cf->east, cf->south);
			break;
		case T_CELLSTATE:
			cstate = (P_CELLSTATE*)p;
			if (board != NULL)
				board->cell(cstate->cx, cstate->cy)->setState( (CELL_STATE)cstate->state, cstate->param );
			break;
		case T_CELLSTEP:
			cstep = (P_CELLSTEP*)p;
			if(board != NULL)
				board->cell(cstep->cx,cstep->cy)->step();
			break;
	}
	
}

void DDClient::onLostDevice(){
	console->onLostDevice();	
}

void DDClient::onResetDevice(){	
	//Set up the light
	D3DLIGHT8 light;
	ZeroMemory( &light, sizeof(D3DLIGHT8) );
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Direction = D3DXVECTOR3(-1,-4,1);
	pd3dDevice->SetLight( 0, &light );
	pd3dDevice->LightEnable( 0, TRUE);

	console->onResetDevice();
}

void DDClient::drawScene(){
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
	
	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ); 
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
	
				pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
				
				//mip mapping
				pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);

	m_cWorld.apply( pd3dDevice );
	if (board != NULL){
		board->render(&m_cWorld);
	}

	m_cHud.apply( pd3dDevice );

/*	if( board != NULL ){
		scores->render(&m_cHud);
		if (levelText != NULL)
			levelText->render(&m_cHud);
		if (info != NULL)
			info->render(&m_cHud);
	}
	if (messageText != NULL)
		messageText->render(&m_cHud);*/
	if (sum != NULL)
		sum->render(&m_cHud);

	if ( ui != NULL )
		ui->render(&m_cHud);

	if (console != NULL)
		console->render(&m_cHud);
}

#define MOVE_STEP 0.1f
#define ROTATE_STEP 0.1f
void DDClient::inputCB(BOOL* input){
	for (int i = 0; i < 10; i++){
		process();
	}
	
	if (board != NULL)
		board->tick(getFrameTime());

	if (modelSelect->getObject() != NULL)
		((Player*)(modelSelect->getObject()))->tick(getFrameTime());

	static int msgcnt = 0;
	msgcnt+=getFrameTime();
	if ( messageTime > 0 && msgcnt > 100 ){
		messageTime--;
		msgcnt = 0;
	}
	if ( messageTime <= 0 )
		messageText->printf("");

	std::map<int, Player*>::iterator it = players.begin();
	while (it != players.end() ){
		(it->second)->tick(getFrameTime());
		it++;
	}

	//process input
	CELL_DIR move;
	if ( input[INPUT_1UP] ){
		move = NORTH;
	} else if( input[INPUT_1DOWN] ){
		move = SOUTH;
	} else if( input[INPUT_1RIGHT] ){
		move = EAST;
	} else if( input[INPUT_1LEFT] ){
		move = WEST;
	} else {
		move = NONE;
	}
	if ( move != lastMove && board != NULL && getUI() == gameUI ){
		//send the new move only if it has changed
		send( (PACKET*)&P_CLIENTSTATE(move) );
	}
	lastMove = move;

}


Player* DDClient::player(int n){
	std::map<int, Player*>::iterator it;
	it = players.find(n);
	if ( it != players.end() )
		return (it->second);
	else
		return NULL;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
		DDClient* g = new DDClient("Daemon Dice", hInstance);
		g->run();
		delete g;
		return 0;
}


const GUID APPGUID = {0xa64c477c, 0x1f5f, 0x5321, {0xcb, 0xce, 0xe7, 0x3c, 0x13, 0xcd, 0xfb, 0x2f}};
void inputParamCB(int &numActions, GUID &DDClientguid, DWORD &genre, DIACTION* &actions, TCHAR name[MAX_PATH]){
	numActions	= NUM_INPUT_ACTIONS;
	DDClientguid	= APPGUID;
	genre		= _GENRE;
	actions		= rgGameAction;
	_tcscpy(name, _T("Daemon Dice Client") );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Console commands
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void DDClient::setupConsole(){
	console = new Console( this );
	console->setTexture(loadTexture( "textures/console.dds" ));

	console->registerConsoleCommand(
		"inputconf", 
		(CONSOLE_CALLBACK_OBJECT)dxs, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::inputConf));
	console->registerConsoleCommand(
		"fullscreen", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::setFullScreen));
	console->registerConsoleCommand(
		"wire", 
		(CONSOLE_CALLBACK_OBJECT)this,
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::wire));
	console->registerConsoleCommand(
		"quit", 
		(CONSOLE_CALLBACK_OBJECT)dxs, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::quit));
	console->registerConsoleCommand(
		"mute", 
		(CONSOLE_CALLBACK_OBJECT)dxs, 
		(CONSOLE_CALLBACK_FUNCTION)(DXSystem::mute));
	console->registerConsoleCommand(
		"connect", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_connect));
		console->registerConsoleCommand(
	"disconnect", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_disconnect));
	console->registerConsoleCommand(
		"say", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_chat));
	console->registerConsoleCommand(
		DEFAULT_CALLBACK, 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_chat));
	console->registerConsoleCommand(
		"name", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_name));
	console->registerConsoleCommand(
		"model", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_model));
	console->registerConsoleCommand(
		"menu", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_menu));
	console->registerConsoleCommand(
		"resume", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_resume));
	console->registerConsoleCommand(
		"modelview", 
		(CONSOLE_CALLBACK_OBJECT)this, 
		(CONSOLE_CALLBACK_FUNCTION2)(DDClient::cmd_modelview));
}

void DDClient::cmd_connect(int argc, char** argv)
{
	try{
		if (argc == 2){ //Server, default port
			Client::connect(argv[1], DEFAULT_PORT);
		} else if (argc == 3) {
			Client::connect(argv[1], atoi(argv[2]));
		} else {
			console->cprintf("Usage: connect host port");
		}
		//now send join.
		send( (PACKET*)&P_PLAYERJOIN(0, name.c_str(), model.c_str() ) );

	} catch ( const char* e ) {
		console->cprintf("Error connecting: %s", e);
	}
}

void DDClient::cmd_disconnect(int argc, char** argv)
{
	disconnect();
}
void DDClient::cmd_chat(int argc, char** argv)
{

std::string s = "";
	int i = 0;
	if (!strcmp(argv[0], "say"))
		i = 1;
	for (; i < argc; i++)
		s = s + argv[i] + " ";

	// @new - support to server commands
	// if we have a slash then we have a server command
	if( s[0] == '\\' || s[0] == '/' ){
		
		// strip out the slashes
		s = s.substr(1); // remove first character

		// send server command packet
		P_SCOMMAND pc(playerNumber,(char*)s.c_str()); //TODO fill in number
		send((PACKET*)&pc);

	} else {// else, we have a chat

		P_CHAT pc(playerNumber,(char*)s.c_str()); //TODO fill in number
		send((PACKET*)&pc);
	}
	// @end new

}

void DDClient::cmd_name(int argc, char** argv)
{
	if (argc != 2)
		console->cprintf("Usage: name <your new name>");
	else {
		if (connected){	//Send the name change if were connected
			send( (PACKET*)&P_PLAYERNAME(playerNumber, argv[1]) );
			console->cprintf("Sent name change: %s.", argv[1]);
			name = argv[1];
		} else {	//otherwise just change the local copy of the name.
			name = argv[1];
		}
	}
}

void DDClient::cmd_model(int argc, char** argv)
{
	if (argc != 2)
		console->cprintf("Usage: modle <your new model file>");
	else {
		if (connected){	//Send the name change if were connected
			send( (PACKET*)&P_PLAYERMODEL(playerNumber, argv[1]) );
			console->cprintf("Sent model change: %s.", argv[1]);
		} else {	//otherwise just change the local copy of the name.
			model = argv[1];
		}
	}
}

void DDClient::cmd_menu(int argc, char** argv)
{
	console->bind(VK_ESCAPE, "resume");
	if (menuUI->getBG() != NULL )
		delete menuUI->getBG();
	menuUI->setBG(new ScreenShot(this, 128, 128));
	setUI(menuUI);
}

void DDClient::cmd_resume(int argc, char** argv)
{
	if ( board != NULL ){
		setUI(gameUI);
		console->bind(VK_ESCAPE, "menu");
	}
}

void DDClient::cmd_modelview(int argc, char** argv)
{
#define MODEL_NUM 2
	static char* models[MODEL_NUM] =
	{
		"models/akiko.ply",
		"models/wizard.ply"
	};
	static int model = 0;
	static char* params[2];
	params[0] = NULL;
	
	if (argv == 0 || !strcmp(argv[1], "next")){
		model++;
		model = model % MODEL_NUM;
		if (modelSelect->getObject() != NULL )
			delete modelSelect->getObject();
		modelSelect->setObject( new Player(this, -1, "Nobody", models[model] ) );
	} else if (!strcmp(argv[1], "prev")){
		model++;
		model = model % MODEL_NUM;
		if (modelSelect->getObject() != NULL )
			delete modelSelect->getObject();
		modelSelect->setObject( new Player(this, -1, "Nobody", models[model] ) );
	} else if (!strcmp(argv[1], "set")){
		params[1] = models[model];
		cmd_model(2, params);
	}
}