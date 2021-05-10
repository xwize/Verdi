
#ifndef __Verdi_Default_Command_Parser_H__
#define __Verdi_Default_Command_Parser_H__

#include "VerdiPrerequisites.h"
#include "VerdiWorkspaceCommandParser.h"

namespace Verdi {

	class DefaultCommandParser : public Verdi::WorkspaceCommandParser
	{
	private:
		//int automatonAdvance(int stateIn, char charIn,
		//	int* stateOut, char* charOut) const; 
	public:
		Verdi::WorkspaceCommandParser::ParseResult parseCommandString(
			const Verdi::String& str,
			const Verdi::Workspace* workspace,
			Verdi::WorkspaceCommand& cmdOut) const;
	};
}

#endif // __Verdi_Default_Command_Parser_H__