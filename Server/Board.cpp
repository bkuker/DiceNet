// Board.cpp: implementation of the Board class.
//
//////////////////////////////////////////////////////////////////////

#include "Board.h"
#include "DDServer.h"
#include <iostream>
#include <stdio.h>

#define SPEEDPARAM 7.0f
#define LEVELPOINTS 500
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	
Board::Board(DDServer* _dds, int _size):
	dds(_dds),
	size(_size),
	newFreq1(0.5f),
	newFreq2(1.0f),
	level(0)
{
	std::cout << "Created " << size << "x" << size << " board." << std::endl;
	
	Cell::firstFree = NULL;
	Cell::freeCount = 0;
	Cell::fullCount = 0;
	cells = new Cell*[size*size];
	int x;
	for ( x = 0; x < size; x++)
		for( int y = 0; y < size; y++ ){
			cells[x+size*y] = new Cell(dds, x, y);
		}
	for ( x = 0; x < size; x++)
		for( int y = 0; y < size; y++ ){
			cell(x,y)->north = cell(x, y + 1);
			cell(x,y)->south = cell(x, y - 1);
			cell(x,y)->east = cell(x + 1, y);
			cell(x,y)->west = cell(x - 1, y);
		}
}


Board::~Board()
{
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			delete cells[x+size*y];
}

void Board::tellClient(CLIENT* c){
	dds->send(c, (PACKET*)&P_BOARD(size));
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ ){
			cells[x+size*y]->tellClient(c);
		}
	dds->send(c, (PACKET*)&P_MSG(MSG_LEVEL, level));
}



float Board::filled()
{
	return (float)Cell::fullCount / (size*size);
}

void Board::reset()
{
	level = 0;
	Cell::fullCount = 0;
	Player::totalScore = 0;
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			cells[x+size*y]->reset();
}

void Board::fill(float d){
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			if ( (float)rand() / RAND_MAX < d )
				cells[x+size*y]->fill();
}

bool Board::gameOver(){
	return Cell::fullCount == size * size;
}


void Board::tick(int milliseconds)
{
	for ( int x = 0; x < size; x++)
		for( int y = 0; y < size; y++ )
			cells[x+size*y]->tick(milliseconds);


	if ( Player::totalScore >= LEVELPOINTS * level ){
		level++;
		char l[12];
		sprintf( l, "Level: %i", level );
		dds->send(ALL_CLIENTS, (PACKET*)&P_CHAT(0, l));
		dds->send(ALL_CLIENTS, (PACKET*)&P_MSG(MSG_LEVEL, level));
	}

	/*if ( Cell::fullCount == size * size ){
		reset();
		std::cout << "The game is lost." << std::endl;
	}*/
	//Check frequency
	float newFreq;
	if ( filled() > 1.0f - 1.0f/((level+SPEEDPARAM)/SPEEDPARAM) )
		newFreq = newFreq1;
	else
		newFreq = newFreq2;

	static int time = 0;
	time = time + milliseconds;
	if (time > 1000.0f / newFreq && Cell::firstFree != NULL){
		//std::cout << "Try New" << std::endl;
		Cell::firstFree->fill();
		time = 0;//time - 1000.0f / newFreq;
	}

	float r = (float)rand() / RAND_MAX;
	if ( r > 0.5f && Cell::firstFree != NULL)
		Cell::firstFree = Cell::firstFree->nextFree;

}

Cell* Board::cell(int x, int y)
{
	if ( x < 0 || y < 0 || x >= size || y >= size )
		return NULL;
	return cells[x+size*y];
}



