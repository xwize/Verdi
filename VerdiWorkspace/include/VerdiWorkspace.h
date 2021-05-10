
#ifndef __Verdi_Workspace_H__
#define __Verdi_Workspace_H__

#include "VerdiPrerequisites.h"
#include "VerdiWorkspaceCommandDescriptor.h"
#include "VerdiWorkspaceVariable.h"

namespace Verdi {

	/** The workspace acts as a console into which
		variables and commands affecting those variables
		can be registered and executed.
	*/
	class Workspace
	{
	protected:
		WorkspaceVariableNameMap mVariables;
		WorkspaceCommandDescriptorNameMap mCommandDescriptors;
	public:

		/** Default constructor.
		*/
		Workspace();

		/** Default destructor.
		*/
		~Workspace();

		/** Clear the workspace to an empty state.
		*/
		void clear();

		/** Execute a command.
		*/
		String executeCommand(const WorkspaceCommand& cmd);

		/** Get a variable by name.
		*/
		WorkspaceVariable* getVariable(const String& name) const;

		/** Get a variable by name and enforce a type.
		*/
		WorkspaceVariable* getVariable(const String& name, const String& type) const;

		/** Get a command descriptor by name.
		*/
		WorkspaceCommandDescriptor* getCommandDescriptor(const String& name) const;

		/** Register a variable to this workspace.
		*/
		void registerVariable(const String& name, WorkspaceVariable* var);

		/** Register a command to this workspace.
		*/
		void registerCommandDescriptor(const String& name, WorkspaceCommandDescriptor* d);

		/** Destroy a single variable.
		*/
		void destroyVariable(const String& name);

		/** Destroy all variables.
		*/
		void destroyAllVariables();

		/** Destroy all commands.
		*/
		void destroyAllCommandDescriptors();
		
		/** Retrieve the variables.
		*/
		const WorkspaceVariableNameMap& getVariables() const {
			return mVariables;
		}

		/** Retrieve the command descriptors.
		*/
		const WorkspaceCommandDescriptorNameMap& getCommandDescriptors() const {
			return mCommandDescriptors;
		}
	};
}

#endif // __Verdi_Workspace_H__