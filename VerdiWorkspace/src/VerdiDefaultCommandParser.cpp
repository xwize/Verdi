
#include "VerdiDefaultCommandParser.h"
#include "VerdiWorkspace.h"
#include "VerdiWorkspaceCommandDescriptor.h"
#include "VerdiStringUtility.h"

#include "boost/xpressive/xpressive.hpp"

namespace Verdi {

	Verdi::WorkspaceCommandParser::ParseResult DefaultCommandParser::parseCommandString(
		const Verdi::String& strIn,
		const Verdi::Workspace* workspace,
		Verdi::WorkspaceCommand& cmdOut) const
	{
		// Copy of the input string
		Verdi::String str = strIn;

		// Returned result
		Verdi::WorkspaceCommandParser::ParseResult res;

		// For keeping track of arguments
		Verdi::StringVector args;

		// Keeping track of function name
		Verdi::String functionName;

		// Command descriptor
		Verdi::WorkspaceCommandDescriptor* desc = NULL;

		// Define the language 
		Verdi::String nameStr = "([a-z][a-zA-Z0-9]*)";
		Verdi::String argStr = "(([a-zA-Z][a-zA-Z0-9]*)|\".*\")";
		Verdi::String funcStr = nameStr + "\\(((" + argStr + ",)*" + argStr + ")?\\);";

		// Compile the grammar
		boost::xpressive::sregex grmFunc = 
			boost::xpressive::sregex::compile(funcStr);

		// Match the string with the language definition
		boost::xpressive::smatch what;
		if( boost::xpressive::regex_match(str,what,grmFunc) ) {

			// Find the first token (the function name)
			int const subs[] = { 1 };
			boost::xpressive::sregex_token_iterator itr( str.begin(), str.end(), grmFunc, subs );
			boost::xpressive::sregex_token_iterator end;
			
			// Failed ?
			if(itr == end) {
				res.description = "Invalid syntax";
				res.type = WorkspaceCommandParser::ParseResult::RC_COMMAND_NOT_FOUND;
				return res;
			}

			// Extract function name
			functionName = itr->str();

			// Check if it exists
			desc = workspace->getCommandDescriptor(functionName);

			// Not found?
			if(desc == NULL) {
				res.description = "Command not found.";
				res.type = WorkspaceCommandParser::ParseResult::RC_COMMAND_NOT_FOUND;
				return res;
			}

			// Add the function name to the arguments
			args.push_back(functionName);

			// Extract arguments string
			size_t oldSize = str.size();
			size_t funSize = functionName.size();

			// Ignore brackets and terminator
			str = str.substr(funSize+1,(oldSize-funSize)-3);

			// If we can't find a , there is only one argument
			if( str.find_first_of(',') != std::string::npos ) {

				Verdi::StringUtility::split(str,',',args);

				// Clean the strings
				for(size_t i = 0; i < args.size(); ++i) {

					// Remove the delimiters
					if(args[i].at(args[i].size()-1) == ',')
						args[i] = args[i].substr(0,args[i].size()-1);

					// Remove any quotations
					if(args[i].at(0) == '"')
						args[i] = args[i].substr(1,args[i].size()-2);
				}

			} else {
				// Only one argument, add it
				if(!str.empty()) {

					// Remove any quotations
					if(str.at(0) == '"')
						str = str.substr(1,str.size()-2);

					args.push_back(str);
				}
			}

			// All the arguments should now be in args, so we check
			// to see if they are correct in number
			if(args.size() < desc->minNumArguments) {
				res.description = "Too few arguments.";
				res.type = WorkspaceCommandParser::ParseResult::RC_INVALID_NUMBER_OF_ARGUMENTS;
				return res;
			}

			// If we get here it's all good
			res.description = "Parse successful.";
			res.type = Verdi::WorkspaceCommandParser::ParseResult::RC_SUCCESS;

			// Fill out the command
			cmdOut = Verdi::WorkspaceCommand(desc->function, args);

			// Success
			return res;

		}
		
		// If we get here, we fail
		res.description = "Invalid syntax.";
		res.type = Verdi::WorkspaceCommandParser::ParseResult::RC_INVALID_SYNTAX;
		return res;
	}
}