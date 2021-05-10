
#ifndef __Verdi_Workspace_Command_H__
#define __Verdi_Workspace_Command_H__

#include "VerdiPrerequisites.h"

namespace Verdi {
	
	// Forward declaration for workspace
	class Workspace;

	/** Typedef for a workspace command function callback.
	*/
	typedef String(*WorkspaceCommandFunction)(Workspace& workspace,
		const StringVector& args);

	/** The workspace command represents
		an instance of a workspace command callback and its
		arguments.
	*/
	class WorkspaceCommand
	{
	protected:
		StringVector mArguments;
		WorkspaceCommandFunction mFunction;
	public:

		/** Default constructor.
		*/
		WorkspaceCommand() : mFunction(NULL)
		{

		}

		/** Parameterised constructor.
		*/
		WorkspaceCommand(WorkspaceCommandFunction func, const StringVector& args) :
			mFunction(func), mArguments(args)
		{

		}

		/** Default destructor.
		*/
		~WorkspaceCommand()
		{

		}

		/** Executes the command function callback passing
			in the arguments.
		*/
		String execute(Workspace& workspace) const 
		{ 
			assert(mFunction != NULL);
			return mFunction(workspace, mArguments); 
		}
	};
}

#endif // __Verdi_Workspace_Command_H__