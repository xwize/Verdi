
#ifndef __Verdi_Workspace_Command_Descriptor_H__
#define __Verdi_Workspace_Command_Descriptor_H__

#include "VerdiPrerequisites.h"
#include "VerdiWorkspaceCommand.h"

namespace Verdi {

	/** The workspace command descriptor
		encapsulates information about a command
		that can be registered with the system.
		Each descriptor , describes a command in terms of
		its arguments, calling name and a description.
	*/
	class WorkspaceCommandDescriptor
	{
	public:
		String description;
		uint32_t minNumArguments;
		WorkspaceCommandFunction function;
	public:

		/** Parameterised constructor.
		*/
		WorkspaceCommandDescriptor(const String& desc,
			uint32_t numArguments, WorkspaceCommandFunction func) :
			description(desc),  minNumArguments(numArguments), function(func)
		{

		}
	};

	// For passing around mappings of named command descriptors
	typedef std::map<String, WorkspaceCommandDescriptor*> 
		WorkspaceCommandDescriptorNameMap;
}

#endif // __Verdi_Workspace_Command_Descriptor_H__
