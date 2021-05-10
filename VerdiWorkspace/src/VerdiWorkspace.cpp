
#include "VerdiWorkspace.h"
#include "VerdiWorkspaceVariable.h"

namespace Verdi {

	Workspace::Workspace()
	{
		mVariables.clear();
		mCommandDescriptors.clear();
	}

	Workspace::~Workspace()
	{
		destroyAllCommandDescriptors();
		destroyAllVariables();
	}

	void Workspace::clear() 
	{
		destroyAllVariables();
		destroyAllCommandDescriptors();
	}

	WorkspaceVariable* Workspace::getVariable(const String& name) const
	{
		WorkspaceVariable* var = NULL;

		WorkspaceVariableNameMap::const_iterator i = mVariables.find(name);
		if( i != mVariables.end() )
			var = i->second;
		return var;
	}

	WorkspaceVariable* Workspace::getVariable(const String& name, const String& type) const 
	{
		WorkspaceVariable* var = NULL;

		WorkspaceVariableNameMap::const_iterator i = mVariables.find(name);
		if( i != mVariables.end() && i->second != NULL && i->second->getType() == type)
			var = i->second;

		return var;
	}

	WorkspaceCommandDescriptor* Workspace::getCommandDescriptor(const String& name) const
	{
		WorkspaceCommandDescriptor* desc = NULL;

		WorkspaceCommandDescriptorNameMap::const_iterator i = 
			mCommandDescriptors.find(name);

		if( i != mCommandDescriptors.end() )
			desc = i->second;
		return desc;
	}

	void Workspace::destroyAllCommandDescriptors()
	{
		WorkspaceCommandDescriptorNameMap::iterator i = mCommandDescriptors.begin();
		for( ; i != mCommandDescriptors.end(); i++) {
			WorkspaceCommandDescriptor* desc = i->second;
			delete desc;
		}

		mCommandDescriptors.clear();
	}

	void Workspace::registerCommandDescriptor(const String& name,
		WorkspaceCommandDescriptor* d)
	{
		assert( mCommandDescriptors.find(name) == mCommandDescriptors.end() );
		mCommandDescriptors[name] = d;
	}

	void Workspace::registerVariable(const String& name,
		WorkspaceVariable* var)
	{
		assert( mVariables.find(name) == mVariables.end() );
		mVariables[name] = var;
	}

	void Workspace::destroyVariable(const String& name)
	{
		WorkspaceVariable* var = NULL;

		WorkspaceVariableNameMap::const_iterator i = 
			mVariables.find(name);

		if( i != mVariables.end() )
			var = i->second;

		delete var;
		mVariables.erase(i);
	}

	void Workspace::destroyAllVariables()
	{
		WorkspaceVariableNameMap::iterator i = mVariables.begin();
		for( ; i != mVariables.end(); i++) {
			WorkspaceVariable* var = i->second;
			delete var;
		}

		mVariables.clear();
	}

	Verdi::String Workspace::executeCommand(const WorkspaceCommand& cmd)
	{
		return cmd.execute((*this));
	}

}