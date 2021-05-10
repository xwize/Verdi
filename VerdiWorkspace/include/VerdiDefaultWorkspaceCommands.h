
#ifndef __Verdi_Default_Workspace_Commands_H__
#define __Verdi_Default_Workspace_Commands_H__

#include "VerdiPrerequisites.h"
#include "VerdiWorkspace.h"

namespace Verdi {

	class DefaultWorkspaceCommands
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
}

#endif // __Verdi_Default_Workspace_Commands_H__