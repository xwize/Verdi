#ifndef WORKSPACECOMMANDS_H
#define WORKSPACECOMMANDS_H

#include "application.h"

class WorkspaceCommands
{
public:
    WorkspaceCommands();

    static Verdi::String commandFunc_tfcreate(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_help(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_quit(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_tfhist(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_listvars(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_dsopen(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_clear(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_mmcreate(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_dsgrads(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_iter(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_simopen(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_pwd(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_swd(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_tfopen(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static Verdi::String commandFunc_tfsave(Verdi::Workspace& workspace,
        const Verdi::StringVector& args);

    static void registerCommands(Verdi::Workspace* workspace);
};

#endif // WORKSPACECOMMANDS_H
