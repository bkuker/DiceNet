// Console.h: interface for the Console class.
//
//////////////////////////////////////////////////////////////////////

// #if !defined(AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_)
// #define AFX_CONSOLE_H__5D1515AB_11E2_41B7_945E_33A6229EBD13__INCLUDED_

#ifndef clsSconsole
#define clsSconsole

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Disable long identifier warnings -- makes stl useable
#pragma warning (disable: 4786)

#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


// #include <windows.h>

#include <stdio.h>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <iostream>



// class Sconsole;

//A class and typedef for console callbacks
#define DEFAULT_CALLBACK "__default_callback_string__"
class HACK {};
class HACK2 {};
class CONSOLE_CALLBACK_CLASS {};
class CONSOLE_CALLBACK_CLASS2 : public HACK, public HACK2 {};
typedef CONSOLE_CALLBACK_CLASS* CONSOLE_CALLBACK_OBJECT;
typedef void (CONSOLE_CALLBACK_CLASS::*CONSOLE_CALLBACK_FUNCTION)(int, char**);
typedef void (CONSOLE_CALLBACK_CLASS2::*CONSOLE_CALLBACK_FUNCTION2)(int, char**);
typedef void (*STATIC_CONSOLE_CALLBACK_FUNCTION)(int, char**);

struct CONSOLE_CALLBACK{
	CONSOLE_CALLBACK_OBJECT object;
	union{
		CONSOLE_CALLBACK_FUNCTION func;
		STATIC_CONSOLE_CALLBACK_FUNCTION sfunc;
		CONSOLE_CALLBACK_FUNCTION2 func2;
	};
};


class Sconsole;

class Sconsole_helper
{
public:

	Sconsole_helper(Sconsole* _c){ c = _c; };
	void processConsoleCmd(char *);
	void aliasCommand(int argc, char** argv);
	void bindCommand(int argc, char** argv);
	Sconsole* c;
};

class Sconsole  
{


friend Sconsole_helper;

public:

	
	Sconsole();
	virtual ~Sconsole();
	

//Console functions
public:

	virtual void setupCommands();
	
	void registerConsoleCommand( std::string, CONSOLE_CALLBACK_OBJECT, CONSOLE_CALLBACK_FUNCTION );
	void registerConsoleCommand( std::string, CONSOLE_CALLBACK_OBJECT, CONSOLE_CALLBACK_FUNCTION2 );
	void registerConsoleCommand( std::string, STATIC_CONSOLE_CALLBACK_FUNCTION );

	void unregisterConsoleCommand( std::string );
	void processConsoleLine(const char *);

	void bind ( int key, std::string command ){ consoleBindings[key] = command; };

public:
	
	// public for server usage
	void processConsoleCmd(char *);
	void aliasCommand(int argc, char** argv);
	void setCommand(int argc, char** argv);

	

//Private functions
protected:
	
	

	void bindCommand(int argc, char** argv);

	virtual void consoleOutput(std::string message); // to partially abstract cprintf and packet sending

	

//Console vars
protected:

	
	std::map<std::string, CONSOLE_CALLBACK> consoleMap;
	std::map<std::string, std::string> consoleAliases;
	std::map<std::string, std::string> consoleVariables;
	std::map<int, std::string> consoleBindings;

	

	std::string commandToBind;
	int bindKey;

	Sconsole_helper* ch;


};

#endif
