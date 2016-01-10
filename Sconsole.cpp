// Console.cpp: implementation of the Console class.
//
//////////////////////////////////////////////////////////////////////

#include "Sconsole.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Sconsole::Sconsole()
{
	//Console Setup
	commandToBind = "";
	bindKey = 0;

	//Attach some console callbacks
	ch = new Sconsole_helper(this);

	/* register commands here
	Note that the way to setup commands is like this: the setupCommands method 
	is virtual, so any commands you setup here but don't want in a subclass you 
	can just override.  Any subclass commands can be placed in the constructor.  

	register command example:

	registerConsoleCommand("console", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(Console_helper::setConsoleStatus));
	

	*/

	registerConsoleCommand("set", (CONSOLE_CALLBACK_OBJECT)this, (CONSOLE_CALLBACK_FUNCTION)(Sconsole::setCommand));

	setupCommands();
		
}

Sconsole::~Sconsole()
{
	delete ch; // delete console helper object pointer
}

//////////////////////////////////////////////////////////////////////
// Console functions
//////////////////////////////////////////////////////////////////////
void Sconsole::registerConsoleCommand( std::string s, CONSOLE_CALLBACK_OBJECT o, CONSOLE_CALLBACK_FUNCTION f){
	CONSOLE_CALLBACK cc;
	cc.func = f;
	cc.object = o;
	consoleMap[s] = cc;
}

void Sconsole::registerConsoleCommand( std::string s, CONSOLE_CALLBACK_OBJECT o, CONSOLE_CALLBACK_FUNCTION2 f){
	CONSOLE_CALLBACK cc;
	cc.func2 = f;
	cc.object = o;
	consoleMap[s] = cc;
}

void Sconsole::registerConsoleCommand( std::string s, STATIC_CONSOLE_CALLBACK_FUNCTION f){
	CONSOLE_CALLBACK cc;
	cc.sfunc = f;
	cc.object = NULL;
	consoleMap[s] = cc;
}

void Sconsole::unregisterConsoleCommand( std::string s ){
	consoleMap.erase(s);
}

#define MAX_ARGS	100
void Sconsole::processConsoleLine(const char *cmd){
	int		argc;
	char	*argv[MAX_ARGS];
	char	*command, *cmem;
	
	if(!cmd)
		return;

#ifdef WIN32
	command = cmem = _strdup(cmd);
#else
	command = cmem = strdup(cmd);
#endif

	//chop off whitespace and quotes
	int qcount = 0;
	while( *command == ' ' || *command == '"'){
		if (*command == '"')
			qcount++;
		command++;
	}
	while( qcount && (*(command + strlen(command)-1) == ' ' || *(command + strlen(command)-1) == '"')){
		if ( *(command + strlen(command)-1) == '"' ){
			qcount--;
		}
		*(command + strlen(command)-1) = NULL;
	}

	//chop into args, honoring quotes
	argc = 1;
	argv[0] = command;
	bool quote = false;
	int len=strlen(command);
	int i;
	for (i = 0; i < len; i++){
		if (command[i] == '"'){
			quote = !quote;
		}
		if ( command[i] == ';' && !quote){
			command[i] = NULL;
			if (command[i+1] != ';' && argc < MAX_ARGS){
				argv[argc] = command + i + 1;
				argc++;
			}
		}
	}
	if (quote){

//		cprintf("Error: Close your quotes: %s", command);

		return;
	}

	for ( i = 0; i < argc; i++){
		processConsoleCmd(argv[i]);
	}

	free(cmem);
}


void Sconsole::processConsoleCmd(char *command)
{
#define MAX_DEPTH 10


	int		argc;
	const char	*argv[MAX_ARGS];
	
	if(!command)
		return;


	argc = 1;
	argv[0] = strtok(command, " ");

	while(argv[argc-1] && argc <= MAX_ARGS){
		argv[argc++] = strtok(NULL, " ");
	}
	argc--;

	if (!argv[0])
		return;
	//Next process the input

	///Look through for variables
	for (int i = 0; i < argc; i++){
		if ( argv[i][0] == '$' ){
			if ( consoleVariables.find(argv[i]+1) != consoleVariables.end() ){
				argv[i] = consoleVariables.find(argv[i]+1)->second.c_str();
			}
		}
	}

	//check for an alias
	//check our alias recursion depth
	static int depth = 0;
	depth++;
	if ( depth >= MAX_DEPTH ){
		// Exceeded alias depth

		depth = 0;
		return;
	}
	std::map<std::string, std::string>::iterator aliasItr;
	aliasItr = consoleAliases.find(argv[0]);
	if ( aliasItr != consoleAliases.end() ){
		std::string cmd = aliasItr->second;
		int len = cmd.length();
		char *copy = new char[len+1];
		strcpy( copy, cmd.c_str() );
		processConsoleLine( copy );
		delete [] copy;
		return;
	}
	depth--;

	//check for a callback and call it if it exists
	std::map<std::string, CONSOLE_CALLBACK>::iterator callbackPair;
	callbackPair = consoleMap.find(argv[0]);
	if ( callbackPair == consoleMap.end() ){
		//if there is no specific callback check for a default	
		callbackPair = consoleMap.find(DEFAULT_CALLBACK);
		if ( callbackPair == consoleMap.end() )

		// should return information to the client probably.  
			0;

	}

	//check to see if we have found a callback, if so call it

	if ( callbackPair != consoleMap.end()){
		CONSOLE_CALLBACK cc = callbackPair->second;
		if( cc.object && cc.func ){
			(*(cc.object).*(cc.func))(argc, (char**)argv);
		}
		else if ( cc.object == NULL && cc.sfunc ){
			(cc.sfunc)(argc, (char**)argv);
		}
	} else {
		
		0;
		// no such callback

	}
}

// important
void Sconsole::aliasCommand(int argc, char** argv)
{
	if (argc < 2){
		
		// not being used right
	
		0;
	
	} else if (argc == 2){
		
		// not aliased
		
		consoleAliases.erase(argv[1]);
	} else {
		//make into a single string
		std::string cmd;
		int i;
		for (i = 2; i < (argc - 1); i++){
			cmd += argv[i];
			cmd += " ";
		}
		cmd += argv[i];
		
		consoleAliases[argv[1]] = cmd;
	}
}

// important
void Sconsole::bindCommand(int argc, char** argv)
{
	if (argc == 1){

		// unbind

		consoleAliases.erase(argv[1]);
	} else {
		//make into a single string
		std::string cmd;
		int i;
		for (i = 1; i < (argc - 1); i++){
			cmd += argv[i];
			cmd += " ";
		}
		cmd += argv[i];

		commandToBind = cmd;
		bindKey = 1;
	}
}

// important
void Sconsole::setCommand(int argc, char** argv)
{
	if (argc == 1){
		std::map<std::string, std::string>::iterator i;
		consoleOutput("Variables set:");
		for(i = consoleVariables.begin(); i != consoleVariables.end(); i++)
			consoleOutput(i->first + "          " + i->second);
	} else if (argc != 3){
		consoleOutput("Usage: set var value");
	} else {
		consoleVariables[argv[1]] = argv[2];
	}
}


// this setups up the commands for the server/client.  It's virtual so that 
// the client can setup it's own callbacks.
void Sconsole::setupCommands(){

	// we will use the alias command to test server alias functionality.
	registerConsoleCommand("alias", (CONSOLE_CALLBACK_OBJECT)ch, (CONSOLE_CALLBACK_FUNCTION)(&Sconsole_helper::aliasCommand));

	// 
	// note: you can also setup commands in the constructor for DDServer since it 
	// inherits all of this functionality.  That's probably the preferred way to do it 
	// if you have callbacks to functionality that only exists in the server class.
	// 

}


// todo: remove
void Sconsole::consoleOutput(std::string message){
}


////////////////
//Console Helper


void Sconsole_helper::aliasCommand(int argc, char** argv)
{
	c->aliasCommand(argc, argv);
}

void Sconsole_helper::bindCommand(int argc, char** argv)
{
	c->bindCommand(argc, argv);
}
