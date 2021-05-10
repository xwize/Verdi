
#include "SimpleWorkspaceCommands.h"
#include "SimpleWorkspaceVariables.h"

Verdi::String SimpleWorkspaceCommands::commandFunc_help(Verdi::Workspace& workspace,
	const Verdi::StringVector& args)
{
	Verdi::String res = "";

	if(args.size() == 1) {

		const Verdi::WorkspaceCommandDescriptorNameMap& cds = 
			workspace.getCommandDescriptors();

		Verdi::WorkspaceCommandDescriptorNameMap::const_iterator i = cds.begin();
		for( ; i != cds.end(); i++)
		{
			if(!res.empty())
				res += "\n";

			res += (i->first);
		}
	}

	if(args.size() >= 2) {
		Verdi::WorkspaceCommandDescriptor* cds = workspace.getCommandDescriptor(args[1]);
		if(cds != NULL) {
			res += cds->description;
		} else res += "Command " + args[1] + " does not exist.";
	}

	return res;
}

Verdi::String SimpleWorkspaceCommands::commandFunc_quit(Verdi::Workspace& workspace,
	const Verdi::StringVector& args)
{
	exit(0);
	return "";
}

Verdi::String SimpleWorkspaceCommands::commandFunc_listvars(Verdi::Workspace& workspace,
	const Verdi::StringVector& args)
{
	Verdi::String res = "";

	if(args.size() == 1) {
		const Verdi::WorkspaceVariableNameMap& vars = workspace.getVariables();
		Verdi::WorkspaceVariableNameMap::const_iterator i = vars.begin();
		for( ; i != vars.end(); ++i)  {

			if(!res.empty())
				res += "\n";

			res += (i->first + " : " + i->second->getType());
		}
	}

	if(args.size() >= 2)
	{
		Verdi::WorkspaceVariable* var = workspace.getVariable(args[1]);
		if(var != NULL)
			res += (var->getDataSummary());
	}

	if(res.empty())
		res = "No variables.";

	return res;
}

Verdi::String SimpleWorkspaceCommands::commandFunc_dsopen(Verdi::Workspace& workspace,
	const Verdi::StringVector& args)
{
	if( workspace.getVariable(args[1]) != NULL)
		return "A variable already exists with that name.";

	Verdi::DataSetReaderPtr reader(NULL);
	Verdi::DataSetReaderFactory factory;

	if( args[3] == "raw")
		reader = factory.getDataSetReader(Verdi::DataSetReaderFactory::DSRF_RAW);
	else if( args[3] == "pvm")
		reader = factory.getDataSetReader(Verdi::DataSetReaderFactory::DSRF_PVM);

	if(reader.get() == NULL)
		return "Invalid file type specified, try pvm or raw.";

	Verdi::DataSet* dataSet = NULL;
	try {
		dataSet = reader->readFromFile(args[2], NULL);
	} catch(Verdi::Exception e)
	{
		return "Failed to load file: " + args[2] + ".";
	}

	DataSetVariable* dsv = new DataSetVariable(dataSet,args[2]);
	workspace.registerVariable(args[1],dsv);

	return "Data set successfully loaded.";
}

Verdi::String SimpleWorkspaceCommands::commandFunc_clear(Verdi::Workspace& workspace,
	const Verdi::StringVector& args)
{
	if(args.size() == 1) {
		workspace.destroyAllVariables();
		return "All variables cleared.";
	}

	if(args.size() == 2) {
		workspace.destroyVariable(args[1]);
		return args[1] + " cleared.";
	}

	return "";
}

Verdi::String SimpleWorkspaceCommands::commandFunc_rscreate(Verdi::Workspace& workspace,
	const Verdi::StringVector& args)
{
	Verdi::RenderStrategy* rs = NULL;

	if( workspace.getVariable(args[1]) != NULL)
		return "A variable already exists with that name.";

	if( args[2] == "RayCastStrategy")
		rs = new Verdi::RayCastStrategy();

	if( args[2] == "TexSliceStrategy")
		rs = new Verdi::TexSliceStrategy();

	if(rs == NULL)
		return "Invalid type specified, try either RayCastStrategy or TexSliceStrategy";

	RenderStrategyVariable* rsv = new RenderStrategyVariable(rs,args[2]);
	workspace.registerVariable(args[1],rsv);

	return "Render strategy successfully created.";
}

void SimpleWorkspaceCommands::registerCommands(Verdi::Workspace* workspace)
{
	workspace->registerCommandDescriptor("help", new Verdi::WorkspaceCommandDescriptor(
		"Displays a list of commands or a commands description.",0,commandFunc_help));

	workspace->registerCommandDescriptor("who", new Verdi::WorkspaceCommandDescriptor(
		"Lists the variables in the workspace.",0,commandFunc_listvars));

	workspace->registerCommandDescriptor("dsopen", new Verdi::WorkspaceCommandDescriptor(
		"Open a data set from file. Format: dsopen(name,filename,filetype, ...).",3,
		commandFunc_dsopen));

	workspace->registerCommandDescriptor("rscreate", new Verdi::WorkspaceCommandDescriptor(
		"Create a render strategy. Format: rscreate(name,type,filename).",2,
		commandFunc_rscreate));

	workspace->registerCommandDescriptor("clear", new Verdi::WorkspaceCommandDescriptor(
		"Clears all variables, or a single variable if a name is provided",0,
		commandFunc_clear));

	workspace->registerCommandDescriptor("quit", new Verdi::WorkspaceCommandDescriptor(
		"Quit the program.",0,commandFunc_quit));
}