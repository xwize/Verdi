
#ifndef __Simple_Command_Parser_H__
#define __Simple_Command_Parser_H__

#include "Prerequisites.h"

class SimpleCommandParser : public Verdi::WorkspaceCommandParser
{
private:
	int automatonAdvance(int stateIn, char charIn,
		int* stateOut, char* charOut) const; 
public:
	Verdi::WorkspaceCommandParser::ParseResult parseCommandString(
		const Verdi::String& str,
		const Verdi::Workspace* workspace,
		Verdi::WorkspaceCommand& cmdOut) const;
};

#endif // __Simple_Command_Parser_H__