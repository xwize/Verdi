
#include "SimpleCommandParser.h"

#define AUTO_ADVANCE_FAIL			0
#define AUTO_ADVANCE_SUCCESS		1
#define AUTO_ADVANCE_SUCCESS_STORE	2

#define AUTO_STATE_COMMAND_NAME		0
#define AUTO_STATE_FIRST_ARGCHAR	1
#define AUTO_STATE_ARGCHAR			2
#define AUTO_STATE_LITERAL			3
#define AUTO_STATE_TERMINATOR		4
#define AUTO_STATE_ACCEPTANCE		5

Verdi::WorkspaceCommandParser::ParseResult SimpleCommandParser::parseCommandString(
	const Verdi::String& str,
	const Verdi::Workspace* workspace,
	Verdi::WorkspaceCommand& cmdOut) const
{
	int state = AUTO_STATE_COMMAND_NAME;
	int acceptanceState = AUTO_STATE_ACCEPTANCE;

	Verdi::String currentString = "";
	std::vector<Verdi::String> stringList;

	Verdi::WorkspaceCommandParser::ParseResult res;

	for(int i = 0; i < (int)str.size(); ++i)
	{
		char ch = str[i];
		char newCh = ch;

		int newState = state;

		int autoResult = automatonAdvance(state, ch, &newState, &newCh);

		if( autoResult == AUTO_ADVANCE_FAIL ) {
			res.type = Verdi::WorkspaceCommandParser::ParseResult::RC_INVALID_SYNTAX;
			res.description = "Invalid syntax";
			return res;
		}

		if(newCh != 0)
			currentString += newCh;

		if(autoResult == AUTO_ADVANCE_SUCCESS_STORE && 
			currentString.size() > 0) {
			stringList.push_back(currentString);
			currentString = "";
		}

		state = newState;
	}

	if(state != acceptanceState || stringList.size() < 1) {
		res.type = Verdi::WorkspaceCommandParser::ParseResult::RC_INVALID_SYNTAX;
		res.description = "Invalid syntax";
		return res;
	}

	Verdi::String commandName = stringList[0];
	Verdi::WorkspaceCommandDescriptor* desc = 
		workspace->getCommandDescriptor(commandName);

	if(desc == NULL) {
		res.type = Verdi::WorkspaceCommandParser::ParseResult::RC_COMMAND_NOT_FOUND;
		res.description = "Command not found";
		return res;
	}

	if(desc->minNumArguments > (stringList.size()-1) ) {
		res.type = Verdi::WorkspaceCommandParser::ParseResult
			::RC_INVALID_NUMBER_OF_ARGUMENTS;
		res.description = "Invalid number of arguments";
		return res;
	}

	cmdOut = Verdi::WorkspaceCommand(desc->function,stringList);

	res.type = Verdi::WorkspaceCommandParser::ParseResult::RC_SUCCESS;
	res.description = "Parse successful";
	return res;
}

int SimpleCommandParser::automatonAdvance(int stateIn, char charIn,
	int* stateOut, char* charOut) const
{
	(*charOut) = charIn;
	(*stateOut) = stateIn;

	if(stateIn == AUTO_STATE_COMMAND_NAME) {

		if(charIn >= 'a' && charIn <= 'z') {
			return AUTO_ADVANCE_SUCCESS;
		}

		if(charIn == '(') {
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_FIRST_ARGCHAR;
			return AUTO_ADVANCE_SUCCESS_STORE;
		}

		if(charIn == ';') {
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_ACCEPTANCE;
			return AUTO_ADVANCE_SUCCESS_STORE;
		}
	}

	if(stateIn == AUTO_STATE_FIRST_ARGCHAR) {
		if(charIn >= 'a' && charIn <= 'z')
		{
			(*stateOut) = AUTO_STATE_ARGCHAR;
			return AUTO_ADVANCE_SUCCESS;
		}

		if(charIn == '"') {
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_LITERAL;
			return AUTO_ADVANCE_SUCCESS;
		}
	}

	if(stateIn == AUTO_STATE_ARGCHAR) {

		if( (charIn >= 'a' && charIn <= 'z') ||
			(charIn >= 'A' && charIn <= 'Z') ||
			(charIn >= '0' && charIn <= '9')) {
			return AUTO_ADVANCE_SUCCESS;
		}
		
		if(charIn == ',') {
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_FIRST_ARGCHAR;
			return AUTO_ADVANCE_SUCCESS_STORE;
		}

		if(charIn == ')') {
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_TERMINATOR;
			return AUTO_ADVANCE_SUCCESS_STORE;
		}
	}

	if(stateIn == AUTO_STATE_LITERAL) {

		if(charIn == '"') {
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_ARGCHAR;
			return AUTO_ADVANCE_SUCCESS;
		}

		return AUTO_ADVANCE_SUCCESS;
	}

	if(stateIn == AUTO_STATE_TERMINATOR) {

		if(charIn == ';')
		{
			(*charOut) = 0;
			(*stateOut) = AUTO_STATE_ACCEPTANCE;
			return AUTO_ADVANCE_SUCCESS;
		}
	}

	return AUTO_ADVANCE_FAIL;
}
