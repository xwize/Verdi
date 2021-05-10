#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "consolewidget.h"
#include "workspacewidget.h"
#include "scenewidget.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateFromModel();
    void setRendererDimensions(int w, int h);

    int getRendererWidth() {
        return mRendererWidth;
    }

    int getRendererHeight() {
        return mRendererHeight;
    }

    QMenu* createPopupMenu();

private slots:
    void on_actionConsole_toggled(bool arg1);
    void on_actionWorkspace_toggled(bool arg1);
    void on_actionData_Set_triggered();
    void on_actionReset_triggered();
    void on_action768_x_768_triggered();
    void on_action256_x_256_triggered();
    void on_action512_x_512_triggered();
    void on_action1024_x_1024_triggered();
    void on_actionExit_triggered();
    void on_actionStart_Testing_triggered();
    void on_actionStop_Testing_triggered();

    void on_actionNew_Transfer_Function_triggered();

    void on_actionTransfer_Function_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    int mRendererWidth;
    int mRendererHeight;

};

#endif // MAINWINDOW_H
