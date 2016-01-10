// Player.cpp: implementation of the Player class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Player.h"
#include "DXSystem.h"
#include "Cell.h"
#include "Context.h"
#include "AnimatedMd2.h"
#include "Text.h"
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Vec2 fcns
//////////////////////////////////////////////////////////////////////

Vec2 operator+(Vec2 a, Vec2 b){
	return Vec2(a.x+b.x, a.y+b.y);
}

Vec2 operator-(Vec2 a, Vec2 b){
	return Vec2(a.x-b.x, a.y-b.y);
}

bool operator==(Vec2 a, Vec2 b){
	return a.x==b.x && a.y == b.y;
}

Vec2 operator*(Vec2 a, float k){
	return Vec2(k*a.x, k*a.y);
}

Vec2 operator/(Vec2 a, float k){
	return Vec2(a.x/k, a.y/k);
}

float length(Vec2 a){
	return sqrt( a.x*a.x + a.y*a.y );
}

Vec2 normalize(Vec2 a){
	return a / length(a);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Player::Player(DXSystem* _dxs, int num, std::string _name, std::string _model):
Renderable(_dxs),
playernum(num),
modelname(""),
name(_name),
score(0),
model(NULL),
m_pCell(NULL),
m_pDest(NULL),
up(false),
p_TScore(new Text(_dxs))
{
	setModel(_model);
	rotateYawPitchRoll(0,0,0);
	scale(1,1,1);
	translate(0,0,0);
	p_TScore->printf("%s: %i", name.c_str(), score);
}

Player::~Player()
{
	if (model != NULL)
		delete model;
	if (m_pCell != NULL)
		m_pCell->delPlayer(this);
	if (p_TScore != NULL)
		delete p_TScore;
}

void Player::reset(){
	setScore(0);
	for(int i = 0; i < 7; i++)
		stats[i] = 0;

	m_pCell = NULL;
	m_pDest = NULL;
	up = false;
}

int Player::playerNum(){
	return playernum;
}

std::string Player::getName(){
	return name;
}

void Player::setName(std::string n){
	name = n;
	p_TScore->printf("%s: %i", name.c_str(), score);
}

void Player::setModel(std::string m){
	if ( modelname == m ){
		dxs->debugprintf("Not reloading same ply file.");
		return;
	}

	modelname = m;
	if (model != NULL)
		delete model;
	std::ifstream in(modelname.c_str());
	if(!in){
		dxs->debugprintf("Player file %s not found!", modelname.c_str());
		return;
	}
	std::string header;
	in >> header;
	dxs->debugprintf("Read player header %s", header.c_str() );

	std::string ani, md2, tex;
	in >> md2;
	in >> tex;
	in >> ani;

	model = new AnimatedMd2( dxs, ani, md2, tex );

	float a, b, c;
	in >> header;
	in >> a;
	in >> b;
	in >> c;
	m_v3Color1 = D3DXVECTOR3(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	m_v3Color2 = D3DXVECTOR3(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	model->scale(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	model->translate(a, b, c);
	in >> header;
	in >> a;
	in >> b;
	in >> c;

	model->rotateYawPitchRoll(a, b, c);


	onResetDevice();
	model->setAnimation("stand");

}

void Player::setCell(Cell* _cell){
	//dxs->debugprintf("Player %d got position.", playernum);
	if (m_pCell == NULL){
		m_pCell = _cell;
		m_pDest = _cell;
		_cell->addPlayer(this);
		m_v2Position.x = 0.5f;
		m_v2Position.y = 0.5f;
	} else if (_cell != m_pCell){		//Only do add/delete if need be
		m_v2Position.y -= _cell->y - m_pCell->y;
		m_v2Position.x -= _cell->x - m_pCell->x;
		
		if (m_pCell != NULL)
			m_pCell->delPlayer(this);
		_cell->addPlayer(this);
		m_pCell = _cell;
	}
}

void Player::setCellWarp(Cell* _cell){
	m_v2Position.x = 0.5f;
	m_v2Position.y = 0.5f;
	destEdge = NONE;
	m_pDest = _cell;
		
	//dxs->debugprintf("Player %d got warped.", playernum);

	if (_cell != m_pCell){		//Only do add/delete if need be	
		if (m_pCell != NULL)
			m_pCell->delPlayer(this);
		_cell->addPlayer(this);
		m_pCell = _cell;
	}

	
	if (m_pCell->state == OCCUPIED 
		|| m_pCell->state == RISE2
		|| m_pCell->state == SINK1)
			up = true;
}

void Player::setDest(Cell* _cell, CELL_DIR edge, bool _up){
	//dxs->debugprintf("Player %d got Destination.", playernum);
	m_pDest = _cell;
	destEdge = edge;
	up = _up;
}

void Player::setScore(int s){
	score = s;
	p_TScore->printf("%s: %i", name.c_str(), score);
}

int Player::getScore() const{
	return score;
}

int Player::getNum() const{
	return playernum;
}

void Player::setStats(int* s){
	memcpy(stats, s, STATSIZE * sizeof(int));
}

void Player::tick(int ms)
{
	if (m_pCell != NULL && up)
	{
		if ( m_pCell->state == SINK1 )
			translate(m_v2Position.x-0.5f, (1 - m_pCell->time) + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == RISE1 )//This only makes sense if packets are getting there slowly...
			translate(m_v2Position.x-0.5f, m_pCell->time + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == RISE2 )
			translate(m_v2Position.x-0.5f, m_pCell->time + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == ROLLOUT )
			translate(m_v2Position.x-0.5f, 0.1f * sinf(D3DX_PI * m_pCell->time) + 1.3f, m_v2Position.y-0.5f);
		else
			translate(m_v2Position.x-0.5f, 1.3f, m_v2Position.y-0.5f);
	}  
	else if (m_pCell != NULL)
	{
		if ( m_pCell->state == SINK2 )
			translate(m_v2Position.x-0.5f, (1 - m_pCell->time) + 0.3f, m_v2Position.y-0.5f);
		else if ( m_pCell->state == RISE1 )
			translate(m_v2Position.x-0.5f, m_pCell->time + 0.3f, m_v2Position.y-0.5f);
		else
			translate(m_v2Position.x-0.5f, 0.3f, m_v2Position.y-0.5f);
	} 

	model->tick(ms);

	if ( m_pCell == NULL || m_pDest == NULL)
		return;


	Vec2 dest(0.5f, 0.5f);
	dest.y += m_pDest->y - m_pCell->y;
	dest.x += m_pDest->x - m_pCell->x;
	switch (destEdge)
		{
		case NORTH:
			dest.y += .5;
			break;
		case SOUTH:
			dest.y -= .5;
			break;
		case EAST:
			dest.x += .5;
			break;
		case WEST:
			dest.x -= .5;
			break;
		}

	float d =  (ms / CELL_ROLL_TIME) / 1000.0f;
	Vec2 dd = dest - Vec2(m_v2Position.x,m_v2Position.y);

	//if we are not close to it
	if ( length( dd ) > d ){
		model->setAnimation("walk");
		float theta = atan2(dd.x,dd.y) + D3DX_PI;
		rotateYawPitchRoll(theta,0,0);
		dd = normalize( dd);
		dd = dd * d;
		Vec2 newPos = Vec2(m_v2Position.x,m_v2Position.y) + dd;
		m_v2Position.x = newPos.x; 
		m_v2Position.y = newPos.y;
	} else {
		model->setAnimation("stand");
		m_v2Position.x = dest.x;
		m_v2Position.y = dest.y;
	}
}

void Player::render(Context *c){
	if (model != NULL){
		c->m_mspWorld->Push();
		c->m_mspWorld->MultMatrixLocal( transform() );
		pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
		
		model->render(c);
		
		pd3dDevice->SetMaterial( &mtrl );
		c->m_mspWorld->Pop();
	}
}

void Player::onResetDevice()
{
	ZeroMemory( &m_mtrl, sizeof(D3DMATERIAL8) );
	m_mtrl.Diffuse.r = m_mtrl.Ambient.r = m_v3Color1.x;
	m_mtrl.Diffuse.g = m_mtrl.Ambient.g = m_v3Color1.y;
	m_mtrl.Diffuse.b = m_mtrl.Ambient.b = m_v3Color1.z;
	m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.0f;
}