// ConfigFile.cpp: implementation of the ConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "ConfigFile.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConfigFile::ConfigFile(std::string _filename):
filename(_filename),
curSec(NULL)
{
	char buf[1024];
	buf[1023]=0;
	std::ifstream in(filename.c_str());
	if(!in){
		throw "Config file not found.";
		return;
	}
	std::string line, curS, k, v;
	while (!in.eof()){
		in.getline(buf,1024);
		line = buf;
		std::cout << "Read " << line << std::endl;
		if (line[0] == '['){
			curS = line.substr(1,line.length()-2);
			std::cout << "In section " << curS << std::endl;
		} else {
			k = line.substr(0, line.find(" = ", 0));
			v = line.substr(line.find(" = ", 0)+3, line.length()-(line.find(" = ", 0)+3));
			std::cout << "Key " << k << " Value " << v << std::endl;
			data[curS][k]=v;
		}

	}
		
}

ConfigFile::~ConfigFile()
{
	
}


std::string ConfigFile::getStr( std::string key )
{
	if ( curSec == NULL )
		throw "No section on stack!";
	std::map< std::string, std::string >::iterator i;
	i = curSec->find( key );
	if ( i == curSec->end() )
		throw "Key not found.";
	return i->second;
}

int ConfigFile::getInt( std::string key )
{
	return atoi( getStr(key).c_str() );
}

float ConfigFile::getFloat ( std::string key )
{
	return atof( getStr(key).c_str() );
}

bool ConfigFile::isKey( std::string key ){
	std::map< std::string, std::string >::iterator i;
	i = curSec->find( key );
	return i != curSec->end();

}

void ConfigFile::pushs( std::string _section )
{
	std::map< std::string, std::map< std::string, std::string > >::iterator i;
	i = data.find(_section);
	if ( i == data.end() )
		throw "Section not found!";
	section.push(_section);
	curSec = &(i->second);
}

std::string ConfigFile::tops()
{
	if ( section.empty() )
		throw "No section on stack!";
	return section.top();
}

void ConfigFile::pops()
{
	if ( section.empty() )
		throw "No section on stack!";
	section.pop();
	if (section.empty())
		curSec = NULL;
	else {
		std::map< std::string, std::map< std::string, std::string > >::iterator i;
		i = data.find(section.top());
		curSec = &(i->second);
	}

}

void ConfigFile::setValueStr( std::string key, std::string value )
{
	(*curSec)[key]=value;
}

void ConfigFile::setValueInt( std::string key, int value )
{
	char buf[32];
	sprintf( buf, "%i", value );
	setValueStr( key, buf );
}

void ConfigFile::setValueFlt( std::string key, float value )
{
	std::string s = "";
	s += value;
	setValueStr( key, s );
}


void ConfigFile::write()
{
	
}

void ConfigFile::writeMap()
{
	
}
