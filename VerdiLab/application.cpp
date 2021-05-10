#include "application.h"

#include "consolewidget.h"
#include "workspacewidget.h"
#include "displaywidget.h"
#include "transferfunctionwidget.h"
#include "scenewidget.h"
#include "mainwindow.h"
#include "workspacevariables.h"

#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>

Application::Application(int argc, char** argv) :
    QApplication(argc, argv)
{
    renderSystem = new Verdi::RenderSystem();
    renderContext = new Verdi::RenderContext();
    workspace = new Verdi::Workspace();

    Verdi::String rayCastPath = "../bin/shaders/RayCastStrategy/RayCast.cgfx";
    Verdi::String rayCastGammaPath = "../bin/shaders/RayCastStrategy/RayCastGamma.cgfx";

    //Verdi::String texSlicePath = "../bin/shaders/TexSliceStrategy/TexSlice.cgfx";

    //renderSystem->registerStrategy("Ray-Caster", new Verdi::RayCastStrategy(rayCastPath) );
    renderSystem->registerStrategy("Ray-Caster (Gamma 2.2)",new Verdi::RayCastStrategy(rayCastGammaPath));

   // renderSystem->registerStrategy("RayCastStrategy", new Verdi::RayCastStrategy(rayCastPath) );
   // renderSystem->registerStrategy("TexSliceStrategy", new Verdi::TexSliceStrategy(texSlicePath) );
    renderSystem->registerContext("RenderContext",renderContext);

    renderSystem->useContext("RenderContext");
    renderSystem->useStrategy("Ray-Caster (Gamma 2.2)");

    WorkspaceCommands::registerCommands(workspace);

    sharedGLWidget = NULL;

    mIsTesting = false;
    mTestTimeRemaining = 0.0f;
    mInitialTestTime = 0.0f;
}

Application::~Application()
{
    stopTesting();

    delete renderSystem;
    renderSystem = NULL;

    delete renderContext;
    renderContext = NULL;

    delete workspace;
    workspace = NULL;
}

void Application::updateWidgetsFromModel()
{
    workspaceWidget->updateFromModel();
    sceneWidget->updateFromModel();
    displayWidget->updateFromModel();
    transferFunctionWidget->updateFromModel();
}

void Application::beginTestIteration(int iteration)
{
    if(!mIsTesting)
        return;

    float duration = mTestDuration;

    mTestTimeRemaining = duration;
    mInitialTestTime = duration;
    mNumTestTicks = 0;
    mTestIteration = iteration;

    int newNumIterations = testIterationSteps[iteration];
    vApp->renderContext->setNumIterations(newNumIterations);
    vApp->renderContext->setLightingEnabled(iteration >= MAX_TEST_ITERATIONS/2);
    vApp->updateWidgetsFromModel();

    QTextStream stream(&mTestFile);
    stream << "; Starting Test Iteration: " << iteration << "\n";
}

void Application::endTestIteration()
{
}

bool Application::startTesting(QString fileName, float duration)
{
    mTestFileName = fileName;
    mTestFile.setFileName(mTestFileName);
    if(!mTestFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    mIsTesting = true;
    mTestDuration = duration;

    QString dsname = vApp->displayWidget->getDisplayWidget()->getSelectedDataSetName();
    DataSetVariable* dsvar = (DataSetVariable*)vApp->workspace->getVariable(dsname.toStdString());

    QTextStream stream(&mTestFile);
    stream << "; VerdiLab test started on " << QDateTime::currentDateTime().toString() << "\n";
    stream << "; Duration: " << duration << "\n";
    stream << "; RenderStrategy: " << QString(vApp->renderSystem->getCurrentStrategyName().c_str()) << "\n";

    if(dsvar != NULL) {
        stream << "; DataSet: " << vApp->displayWidget->getDisplayWidget()->getSelectedDataSetName() << "\n";
        stream << "; DataSetFile: " << QString(dsvar->getFileName().c_str()) << "\n";
        stream << "; DataSetSummary: " << QString(dsvar->getDataSummary().c_str()) << "\n";
    }

    stream << "; Internal Width: " << vApp->mainWindow->getRendererWidth() << "\n";
    stream << "; Internal Height: " << vApp->mainWindow->getRendererHeight() << "\n";

    stream << "; Format: <application dt>,<frame dt>,"
           << "<smoothed frame dt>,<smoothed fps>,"
           << "<yaw radians>,<iterations>,<lighting>\n";

    stream.flush();

    int n = 100;
    for(int i = 0; i < MAX_TEST_ITERATIONS/2; i ++) {
        testIterationSteps[i] = n;
        testIterationSteps[i+MAX_TEST_ITERATIONS/2] = n;
        n = n + 300;
    }

    beginTestIteration(0);
    return true;
}

void Application::tickTest(float appDeltaTime,
                           float frameDeltaTime,
                           float avgDeltaTime,
                           float yaw)
{
    if(!mIsTesting)
        return;

    mTestTimeRemaining -= appDeltaTime;
    mNumTestTicks ++;

    if(mTestTimeRemaining <= 0.0f)
    {
        mTestIteration++;

        if(mTestIteration < MAX_TEST_ITERATIONS) {
            endTestIteration();
            beginTestIteration(mTestIteration);
        }
        else {
            endTestIteration();
            stopTesting();
            mainWindow->updateFromModel();
            QMessageBox msg;
            msg.setIcon(QMessageBox::Information);
            msg.setText("Test succesfully finished.");
            msg.exec();
            return;
        }
    }

    QTextStream stream(&mTestFile);
    stream.setRealNumberNotation(QTextStream::FixedNotation);
    stream << appDeltaTime << ","
           << frameDeltaTime << ","
           << avgDeltaTime << ","
           << 1.0f/avgDeltaTime << ","
           << yaw << ","
           << vApp->renderContext->getNumIterations() << ","
           << vApp->renderContext->getLightingEnabled() << "\n";
}

void Application::stopTesting()
{
    if(!mIsTesting)
        return;

    QTextStream stream(&mTestFile);
    stream << "; Test finished. Actual duration: "
           << (mInitialTestTime - mTestTimeRemaining) << "\n"
           << "; Ticks: " << mNumTestTicks << "\n";

    if(mTestFile.isOpen())
        mTestFile.close();

    mIsTesting = false;
    mTestTimeRemaining = 0.0f;
}
