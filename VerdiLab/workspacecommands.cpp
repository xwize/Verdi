#include "workspacecommands.h"
#include "workspacevariables.h"

#ifdef Q_WS_WIN
#include <direct.h>
#else
#include <unistd.h>
#define _getcwd(a,b) getcwd(a,b)
#define _chdir(a) chdir(a)
#endif

#include <stdlib.h>
#include <stdio.h>

#include <QLibrary>
#include <QImageWriter>

WorkspaceCommands::WorkspaceCommands()
{
}

Verdi::String WorkspaceCommands::commandFunc_pwd(Verdi::Workspace &workspace,
                                                 const Verdi::StringVector &args)
{
    return _getcwd(NULL,0);
}

Verdi::String WorkspaceCommands::commandFunc_swd(Verdi::Workspace &workspace,
                                                 const Verdi::StringVector &args)
{
    return _chdir(args[1].c_str()) == 0 ? "Working directory changed." : "Unable to set working directory.";
}

Verdi::String WorkspaceCommands::commandFunc_tfopen(Verdi::Workspace &workspace,
                                                    const Verdi::StringVector &args)
{
    Verdi::TransferFunction* tf = NULL;

    if( workspace.getVariable(args[1]) != NULL)
        return "A variable already exists with that name.";

    int tw = 256;
    int th = 1;

    QImage * image = new QImage();

    if(image->load( QString(args[2].c_str()) ))
    {
        tw = image->width();
        th = image->height();

        tf = new Verdi::TransferFunction(Verdi::Vec4(0.95f,0.95f,0.95f,0.5f),tw,th);

        for(int i = 0; i < tw; ++i)
        {
            for(int j = 0; j < th; ++j)
            {
                QRgb q = image->pixel(i,j);

                Verdi::Vec4 col;
                col.x = (float)qRed(q) / 255.0f;
                col.y = (float)qGreen(q) / 255.0f;
                col.z = (float)qBlue(q) / 255.0f;
                col.w = (float)qAlpha(q) / 255.0f;

                tf->setPixel(i,j,col);
            }
        }

        TransferFunctionVariable* tfv = new TransferFunctionVariable(tf);
        tfv->updateHardwareTransferFunction();
        workspace.registerVariable(args[1],tfv);
        delete image;
        return "Image loaded succesfully.";
    }

    delete image;
    return "Failed to open image.";
}

Verdi::String WorkspaceCommands::commandFunc_tfsave(Verdi::Workspace &workspace,
                                                    const Verdi::StringVector &args)
{
    Verdi::TransferFunction* tf = NULL;

    TransferFunctionVariable* tfv = (TransferFunctionVariable*)workspace.getVariable(args[1]);

    if( tfv == NULL)
        return "A variable doesn't exist with that name.";

    tf = tfv->getTransferFunction();

    int tw = tf->getWidth();
    int th = tf->getHeight();

    assert(tw > 0 && th > 0);

    QImage * image = new QImage(tw,th,QImage::Format_ARGB32);

    for(int i = 0; i < tw; ++i)
    {
        for(int j = 0; j < th; ++j)
        {
            Verdi::Vec4 v = tf->getPixel(i,j);
            int tr = v.x * 255.0f;
            int tg = v.y * 255.0f;
            int tb = v.z * 255.0f;
            int ta = v.w * 255.0f;
            QRgb rgb = qRgba(tr,tg,tb,ta);
            image->setPixel(QPoint(i,j),rgb);
        }
    }

    if(image->save( QString(args[2].c_str()) ))
    {
        delete image;
        return "Image succesfully written.";
    }

    delete image;
    assert( QImageWriter::supportedImageFormats().size() > 0);
    return "Failed to save image.";
}


Verdi::String WorkspaceCommands::commandFunc_tfcreate(Verdi::Workspace& workspace,
    const Verdi::StringVector& args)
{
    Verdi::TransferFunction* tf = NULL;

    if( workspace.getVariable(args[1]) != NULL)
        return "A variable already exists with that name.";


    int tw = 256;
    int th = 1;

    if( args.size() >= 3 )
        tw = Verdi::StringUtility::toInt(args[2]);

    if( args.size() >= 4 )
        th = Verdi::StringUtility::toInt(args[3]);

    if(tw < 32) tw = 32;
    if(th < 1) th = 1;

    tf = new Verdi::TransferFunction(Verdi::Vec4(0.95f,0.95f,0.95f,0.5f),tw,th);

    TransferFunctionVariable* tfv = new TransferFunctionVariable(tf);
    tfv->updateHardwareTransferFunction();
    workspace.registerVariable(args[1],tfv);

    return "Transfer function successfully created.";
}

Verdi::String WorkspaceCommands::commandFunc_tfhist(Verdi::Workspace& workspace,
    const Verdi::StringVector& args)
{
    Verdi::DataSet* ds = NULL;
    DataSetVariable* dsv = NULL;

    Verdi::TransferFunction* tf = NULL;
    TransferFunctionVariable* tfv = NULL;

    if( workspace.getVariable(args[1],"TransferFunction") == NULL)
        return "Transfer function not found with that name.";

    if( workspace.getVariable(args[2],"DataSet") == NULL)
        return "Data set not found with that name.";

    tfv = (TransferFunctionVariable*)workspace.getVariable(args[1],"TransferFunction");
    dsv = (DataSetVariable*)workspace.getVariable(args[2],"DataSet");

    tf = tfv->getTransferFunction();
    ds = dsv->getDataSet();


     for(int k = 0; k < ds->getDepth(); ++k) {
        for(int j = 0; j < ds->getHeight(); ++j) {
              for(int i = 0; i < ds->getWidth(); ++i) {

                float v = ds->getVoxel(i,j,k,0);

                int vx = (v * (float)tf->getWidth());
                int vy = (v * (float)tf->getHeight());

                if(tf->getHeight() == 1)
                    vy = 0;

                Verdi::Vec4 p = tf->getPixel(vx,vy);

                float r = (p.w-0.00001f);
                p.x = 0.5f + sinf( p.x + 10.0f*(v+r) )*0.5f;
                p.y = 0.5f + cosf( p.y + 10.0f*(v+r) )*0.5f;
                p.z = 0.5f + sinf( p.z + p.x + p.y ) * 0.5f;
                p.w = Verdi::Math::clamp(r,0.25f,1.0f);

                tf->setPixel(vx,vy,p);
            }
        }
    }

    tfv->markForUpdate();
    return "Completed.";
}

Verdi::String WorkspaceCommands::commandFunc_help(Verdi::Workspace& workspace,
    const Verdi::StringVector& args)
{
    Verdi::String res = "";

    if(args.size() == 1) {

        const Verdi::WorkspaceCommandDescriptorNameMap& cds =
            workspace.getCommandDescriptors();

        Verdi::WorkspaceCommandDescriptorNameMap::const_iterator i = cds.begin();
        int c = 0;
        for( ; i != cds.end(); i++)
        {
            if(!res.empty() && c%5 == 0)
                res += "\n";

            res += (i->first);
            res += "\t";

            c++;
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

Verdi::String WorkspaceCommands::commandFunc_quit(Verdi::Workspace& workspace,
    const Verdi::StringVector& args)
{
    VERDI_GL_OK();
    vApp->exit(0);
    return "Shuting down...";
}

Verdi::String WorkspaceCommands::commandFunc_mmcreate(Verdi::Workspace& workspace,
   const Verdi::StringVector& args)
{
    if( workspace.getVariable(args[1]) != NULL)
        return "A variable already exists with that name.";

    Verdi::WorkspaceVariable* dsv = workspace.getVariable(args[2]);

    if( dsv != NULL && dsv->getType() == "DataSet" ) {

        Verdi::DataSet* ds = ((DataSetVariable*)dsv)->getDataSet();

        Verdi::MinMaxOctree* mm = new Verdi::MinMaxOctree();
        mm->create(ds,0,5);

        MinMaxOctreeVariable* mov = new MinMaxOctreeVariable(mm);
        workspace.registerVariable(args[1],mov);
        return "Tree successfully created";
    }

    return "Invalid data set specified.";
}

Verdi::String WorkspaceCommands::commandFunc_listvars(Verdi::Workspace& workspace,
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
        {
            res += var->getType() + "\n";
            res += (var->getDataSummary());

            if(var->isFromFile())
                res += "\n" + var->getFileName();
        }
    }

    if(res.empty())
        res = "No variables.";

    return res;
}

Verdi::String WorkspaceCommands::commandFunc_dsopen(Verdi::Workspace& workspace,
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
    {
        return "Invalid file type specified, try pvm or raw.";
    }

    Verdi::DataSet* dataSet = NULL;
    try {
        Verdi::StringMap loadArgs;
        loadArgs["quantise"] = "0";
        if( args.size() > 4)
            loadArgs["quantise"] = args[4];

        dataSet = reader->readFromFile(args[2], &loadArgs);
    } catch(Verdi::Exception e)
    {
        return "Failed to load file: " + args[2] + ".";
    }

    DataSetVariable* dsv = new DataSetVariable(dataSet,args[2]);
    workspace.registerVariable(args[1],dsv);

    return "Data set successfully loaded.";
}

Verdi::String WorkspaceCommands::commandFunc_iter(Verdi::Workspace& workspace,
    const Verdi::StringVector& args)
{
    int numIterations = vApp->renderContext->getNumIterations();
    int newNumIter = Verdi::StringUtility::toInt(args[1]);

    if(newNumIter > 10 && newNumIter <= 2000)
        numIterations = newNumIter;
    else
        return "Out of range.";

    vApp->renderContext->setNumIterations(numIterations);
    vApp->updateWidgetsFromModel();
    return "Iterations set to " + Verdi::StringUtility::toString(newNumIter) + ".";
}

Verdi::String WorkspaceCommands::commandFunc_simopen(Verdi::Workspace &workspace, const Verdi::StringVector &args)
{
    if( workspace.getVariable(args[1]) != NULL)
        return "A variable already exists with that name.";

    QLibrary lib( QString(args[2].c_str()) );
    lib.load();

    if(lib.isLoaded())
    {
        SimulationVariable::SimCreateFunc* pCreate =
                (SimulationVariable::SimCreateFunc*)lib.resolve("simCreate");

        SimulationVariable::SimUpdateFunc* pUpdate =
               (SimulationVariable::SimUpdateFunc*) lib.resolve("simUpdate");

        SimulationVariable::SimDestroyFunc* pDestroy =
                (SimulationVariable::SimDestroyFunc*)lib.resolve("simDestroy");

        if(pCreate && pUpdate && pDestroy)
        {
            SimulationVariable* dsv = new SimulationVariable(pCreate,pUpdate,pDestroy,args[2]);
            workspace.registerVariable(args[1],dsv);
            return "Data set simulation succesfully loaded.";
        }
    }

    return "File is not a valid simulation.";
}

Verdi::String WorkspaceCommands::commandFunc_clear(Verdi::Workspace& workspace,
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

void WorkspaceCommands::registerCommands(Verdi::Workspace *workspace)
{
    Verdi::String desc;

    // Refactor

    desc = "";
    desc += "Description:\n\tDisplays a list of commands or a command description.\n";
    desc += "Format:\n\thelp() or help(<command_name>)";
    workspace->registerCommandDescriptor("help", new Verdi::WorkspaceCommandDescriptor(
        desc,1,commandFunc_help));

    desc = "";
    desc += "Description:\n\tLists the variables in the workspace or print out a summary for a variable.\n";
    desc += "Format:\n\twho() or who(<variable_name>)";
    workspace->registerCommandDescriptor("who", new Verdi::WorkspaceCommandDescriptor(
        desc,1,commandFunc_listvars));

    desc = "";
    desc += "Description:\n\tOpen a data set from a file.\n";
    desc += "Format:\n\tdsopen(<name>,<filename>,<filetype>, <optional_arguments>, ... )";
    workspace->registerCommandDescriptor("dsopen", new Verdi::WorkspaceCommandDescriptor(
        desc,4,commandFunc_dsopen));

    desc = "";
    desc += "Description:\n\tOpen a transfer function from a file.\n";
    desc += "Format:\n\ttfopen(<name>,<filename>)";
    workspace->registerCommandDescriptor("tfopen", new Verdi::WorkspaceCommandDescriptor(
        desc,3,commandFunc_tfopen));

    desc = "";
    desc += "Description:\n\tSave a transfer function to a file.\n";
    desc += "Format:\n\ttfsave(<name>,<filename>)";
    workspace->registerCommandDescriptor("tfsave", new Verdi::WorkspaceCommandDescriptor(
        desc,3,commandFunc_tfsave));

    desc = "";
    desc += "Description:\n\tClears all variables, or a single variable if a name is provided.\n";
    desc += "Format:\n\tclear() or clear(<variable_name>)";
    workspace->registerCommandDescriptor("clear", new Verdi::WorkspaceCommandDescriptor(
        desc,1, commandFunc_clear));

    desc = "";
    desc += "Description:\n\tCreate a new transfer function.\n";
    desc += "Format:\n\ttfcreate(<name>) or tfcreate(<name>,<width>,<height>)";
    workspace->registerCommandDescriptor("tfcreate", new Verdi::WorkspaceCommandDescriptor(
        desc,2,commandFunc_tfcreate));

    desc = "";
    desc += "Description:\n\tModify a transfer function using a histogram of data set.\n";
    desc += "Format:\n\ttfhist(<name>,<dataset>)";
    workspace->registerCommandDescriptor("tfhist", new Verdi::WorkspaceCommandDescriptor(
        desc,3,commandFunc_tfhist));

    desc = "";
    desc += "Description:\n\tCreate a new minmax tree.\n";
    desc += "Format:\n\tmmcreate(<name>,<dataset>)";
    workspace->registerCommandDescriptor("mmcreate", new Verdi::WorkspaceCommandDescriptor(
        desc,3,commandFunc_mmcreate));

    desc = "";
    desc += "Description:\n\tSet the number of iterations.\n";
    desc += "Format:\n\titer(<value>);";
    workspace->registerCommandDescriptor("iter", new Verdi::WorkspaceCommandDescriptor(
        desc,2,commandFunc_iter));

    desc = "";
    desc += "Description:\n\tLoad a simulation from a file.\n";
    desc += "Format:\n\tsimopen(<name>,<sim>)";
    workspace->registerCommandDescriptor("simopen", new Verdi::WorkspaceCommandDescriptor(
        desc,3,commandFunc_simopen));

    desc = "";
    desc += "Description:\n\tSet working directory.\n";
    desc += "Format:\n\tswd(<dir>)";
    workspace->registerCommandDescriptor("swd", new Verdi::WorkspaceCommandDescriptor(
        desc,2,commandFunc_swd));

    workspace->registerCommandDescriptor("pwd", new Verdi::WorkspaceCommandDescriptor(
        "Print working directory.",1,commandFunc_pwd));

    workspace->registerCommandDescriptor("quit", new Verdi::WorkspaceCommandDescriptor(
        "Quit the program.",1,commandFunc_quit));

    /*workspace->registerCommandDescriptor("rscreate", new Verdi::WorkspaceCommandDescriptor(
        "Create a render strategy. Format: rscreate(name,type,filename).",2,
        commandFunc_rscreate));*/
}
