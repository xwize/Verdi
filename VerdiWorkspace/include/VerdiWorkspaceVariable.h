
#ifndef __Verdi_Workspace_Variable_H__
#define __Verdi_Workspace_Variable_H__

#include "VerdiPrerequisites.h"

namespace Verdi {

	/** Represents a variable in the workspace.
		Each variable encapsulates an aspect of
		functionality provided by the framework.
		These variables can be modified through
		the workspace console system.
	*/
	class WorkspaceVariable
	{
	protected:
		String mFileName;
		bool mIsFromFile;
	public:

		/** Parameterised constructor.
		*/
		WorkspaceVariable();

		/** Virtual destructor.
		*/
		virtual ~WorkspaceVariable();

		/** Get the file name of this variable.
		*/
		virtual String getFileName() const { return mFileName; }
			
		/** Get a string summary of the data.
		*/
		virtual String getDataSummary() const = 0;

		/** Get the type of this variable.
		*/
		virtual String getType() const = 0;

		/** Is this variable from a file.
		*/
		virtual bool isFromFile() const { return mIsFromFile; }
	};

	// For passing around mappings of named variables
	typedef std::map<String, WorkspaceVariable*>
		WorkspaceVariableNameMap;
}

#endif // __Verdi_Workspace_Variable_H__