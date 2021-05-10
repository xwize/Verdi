#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QGLWidget>
#include <QFile>
#include <QTextStream>

#include "Verdi.h"
#include "VerdiWorkspace.h"
#include "VerdiWorkspaceVariable.h"
#include "VerdiDefaultWorkspaceCommands.h"
#include "VerdiDefaultWorkspaceVariables.h"

#include "VerdiRayCastStrategy.h"
//#include "VerdiTexSliceStrategy.h"

#include "workspacecommands.h"

class WorkspaceWidget;
class ConsoleWidget;
class DisplayWidget;
class TransferFunctionWidget;
class SceneWidget;
class MainWindow;

#define vApp ((Application*)(qApp))

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int argc, char** argv);
    ~Application();

    void updateWidgetsFromModel();

    bool startTesting(QString fileName, float duration);

    void beginTestIteration(int iteration);
    void endTestIteration();

    void tickTest(float appDeltaTime,
                  float frameDeltaTime,
                  float avgDeltaTime,
                  float yaw);

    void stopTesting();

    float getUnitisedTestTime() {
        return 1.0f - (mTestTimeRemaining/mInitialTestTime);
    }

    bool isTesting() const {
        return mIsTesting;
    }

private:
    QString mTestFileName;
    QFile mTestFile;
    bool mIsTesting;
    int mNumTestTicks;
    float mTestTimeRemaining;
    float mTestDuration;
    float mInitialTestTime;
    static const int MAX_TEST_ITERATIONS = 10;
    int mTestIteration;
    int testIterationSteps[MAX_TEST_ITERATIONS];
public:
    QGLWidget*               sharedGLWidget;
    WorkspaceWidget*         workspaceWidget;
    ConsoleWidget*           consoleWidget;
    DisplayWidget*           displayWidget;
    TransferFunctionWidget*  transferFunctionWidget;
    SceneWidget*             sceneWidget;
    MainWindow*              mainWindow;
public:
    Verdi::Workspace*        workspace;
    Verdi::RenderSystem*     renderSystem;
    Verdi::RenderContext*    renderContext;
signals:
    
public slots:
    
};

#endif // APPLICATION_H
