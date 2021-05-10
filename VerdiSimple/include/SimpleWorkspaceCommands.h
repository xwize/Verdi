
#ifndef __Simple_Workspace_Commands_H__
#define __Simple_Workspace_Commands_H__

#include "Prerequisites.h"

class SimpleWorkspaceCommands
{
protected:
public:

	static Verdi::String commandFunc_help(Verdi::Workspace& workspace,
		const Verdi::StringVector& args);

	static Verdi::String commandFunc_quit(Verdi::Workspace& workspace,
		const Verdi::StringVector& args);

	static Verdi::String commandFunc_listvars(Verdi::Workspace& workspace,
		const Verdi::StringVector& args);

	static Verdi::String commandFunc_dsopen(Verdi::Workspace& workspace,
		const Verdi::StringVector& args);

	static Verdi::String commandFunc_rscreate(Verdi::Workspace& workspace,
		const Verdi::StringVector& args);

	static Verdi::String commandFunc_clear(Verdi::Workspace& workspace,
		const Verdi::StringVector& args);

	static void registerCommands(Verdi::Workspace* workspace);
};

#endif // __Simple_Workspace_Commands_H__