#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "displaywidget.h"
#include "transferfunctionwidget.h"
#include "application.h"
#include "workspacecommands.h"
#include "application.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    vApp->mainWindow = this;

    vApp->displayWidget = new DisplayWidget(this);
    addDockWidget(Qt::TopDockWidgetArea, vApp->displayWidget);

    vApp->sceneWidget = new SceneWidget(this);
    addDockWidget(Qt::TopDockWidgetArea, vApp->sceneWidget);

    vApp->transferFunctionWidget = new TransferFunctionWidget(this);
    addDockWidget(Qt::TopDockWidgetArea, vApp->transferFunctionWidget);

    vApp->consoleWidget = new ConsoleWidget(this);
    addDockWidget(Qt::BottomDockWidgetArea, vApp->consoleWidget );

    vApp->workspaceWidget = new WorkspaceWidget(this);
    addDockWidget(Qt::BottomDockWidgetArea, vApp->workspaceWidget );

    ui->setupUi(this);
    ui->centralWidget->hide();
    //ui->mainToolBar->hide();

    vApp->updateWidgetsFromModel();

    setRendererDimensions(512,512);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QMenu* MainWindow::createPopupMenu()
{
    return NULL;
}

void MainWindow::updateFromModel()
{
    ui->actionStart_Testing->setEnabled( !vApp->isTesting() );
    ui->actionStop_Testing->setEnabled( vApp->isTesting() );
}

void MainWindow::setRendererDimensions(int w, int h)
{
    ui->action768_x_768->setChecked(false);
    ui->action256_x_256->setChecked(false);
    ui->action512_x_512->setChecked(false);
    ui->action1024_x_1024->setChecked(false);

    if(w == 768 && h == 768) {
        mRendererWidth = w;
        mRendererHeight = h;
        ui->action768_x_768->setChecked(true);
    }
    else if(w == 256 && h == 256) {
        mRendererWidth = w;
        mRendererHeight = h;
        ui->action256_x_256->setChecked(true);
    }
    else if(w == 512 && h == 512) {
        mRendererWidth = w;
        mRendererHeight = h;
        ui->action512_x_512->setChecked(true);
    }
    else if(w == 1024 && h == 1024) {
        mRendererWidth = w;
        mRendererHeight = h;
        ui->action1024_x_1024->setChecked(true);
    }
}

void MainWindow::on_actionConsole_toggled(bool arg1)
{
}

void MainWindow::on_actionWorkspace_toggled(bool arg1)
{
}

void MainWindow::on_actionData_Set_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Data Set"), "", tr("Volume Files (*.pvm)"));

    if(fileName != "")
    {
        Verdi::String baseName = "ds";
        Verdi::String name = baseName;

        int c = 1;
        do {
            name = baseName + Verdi::StringUtility::toString(c);
            c++;
        } while(vApp->workspace->getVariable(name) != NULL);

        QString cmd = "dsopen(" + QString(name.c_str()) + ",\"" + fileName + "\",pvm);";
        vApp->consoleWidget->executeCommand(cmd);
    }
}

void MainWindow::on_actionReset_triggered()
{
    int w = mRendererWidth;
    int h = mRendererHeight;

    try {
        printf("Shutting down...");
        vApp->renderSystem->shutdown();
        printf("Initialising...");
        vApp->renderSystem->initialise(w,h);

        int ww = vApp->displayWidget->getDisplayWidget()->geometry().width();
        int wh = vApp->displayWidget->getDisplayWidget()->geometry().height();
        printf("Resizing...");
        vApp->displayWidget->getDisplayWidget()->resizeGL(ww,wh);

    } catch(const Verdi::Exception& e) {

        vApp->renderSystem->shutdown();

        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText( QString( Verdi::StringUtility::toString(e).c_str() ) );
        msg.exec();
    }
}

void MainWindow::on_action768_x_768_triggered()
{
    setRendererDimensions(768,768);
    on_actionReset_triggered();
}

void MainWindow::on_action256_x_256_triggered()
{
    setRendererDimensions(256,256);
    on_actionReset_triggered();
}

void MainWindow::on_action512_x_512_triggered()
{
    setRendererDimensions(512,512);
    on_actionReset_triggered();
}

void MainWindow::on_action1024_x_1024_triggered()
{
    setRendererDimensions(1024,1024);
    on_actionReset_triggered();
}

void MainWindow::on_actionExit_triggered()
{
    vApp->consoleWidget->executeCommand("quit();");
}

void MainWindow::on_actionStart_Testing_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Test"), "", tr("Text Files (*.txt)"));

    if(fileName != "")
    {
        vApp->startTesting(fileName, 15.0f);
        ui->actionStop_Testing->setEnabled(true);
        ui->actionStart_Testing->setEnabled(false);
    }
}

void MainWindow::on_actionStop_Testing_triggered()
{
    vApp->stopTesting();

    ui->actionStart_Testing->setEnabled(true);
    ui->actionStop_Testing->setEnabled(false);
}

void MainWindow::on_actionNew_Transfer_Function_triggered()
{
        Verdi::String baseName = "tf";
        Verdi::String name = baseName;

        int c = 1;
        do {
            name = baseName + Verdi::StringUtility::toString(c);
            c++;
    } while(vApp->workspace->getVariable(name) != NULL);

    QString cmd = "tfcreate(" + QString(name.c_str()) + ");";
    vApp->consoleWidget->executeCommand(cmd);
}

void MainWindow::on_actionTransfer_Function_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Transfer Function"), "", tr("All Files (*.*);;JPEG (*.jpg);;PNG (*.png);;Bitmap (*.bmp)"));

    if(fileName != "")
    {
        Verdi::String baseName = "tf";
        Verdi::String name = baseName;

        int c = 1;
        do {
            name = baseName + Verdi::StringUtility::toString(c);
            c++;
        } while(vApp->workspace->getVariable(name) != NULL);

        QString cmd = "tfopen(" + QString(name.c_str()) + ",\"" + fileName + "\");";
        vApp->consoleWidget->executeCommand(cmd);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About Verdi Lab","\t" + this->windowTitle() +
                       "\t\n\nJames L. Jones 2012 - 2013 @ xwize.wordpress.com");
}
