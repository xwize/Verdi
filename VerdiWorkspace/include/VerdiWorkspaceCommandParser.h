
#ifndef __Verdi_Workspace_Command_Parser_H__
#define __Verdi_Workspace_Command_Parser_H__

#include "VerdiPrerequisites.h"

namespace Verdi {

	// Forward declarations
	class Workspace;
	class WorkspaceCommand;

	/** Interface for a workspace command parser.
	*/
	class WorkspaceCommandParser
	{
	public:
		class ParseResult
		{
		public:
			/** Enumeration of possible parsing results.
			*/
			enum ResultType {
				RC_INVALID_SYNTAX,
				RC_INVALID_NUMBER_OF_ARGUMENTS,
				RC_COMMAND_NOT_FOUND,
				RC_SUCCESS
			};
		public:
			ResultType type;
			String description;
		public:

			ParseResult(ResultType t = RC_SUCCESS, String desc = "") : 
			  type(t), description(desc) {}
		};
	protected:
	public:

		/** Parse an input string using the provided workspace
			to produce an executable command.
		*/
		virtual ParseResult parseCommandString(const String& str,
			const Workspace* workspace,
			WorkspaceCommand& cmdOut) const = 0;
	};
}

#endif // __Verdi_Workspace_Command_Parser_H__
