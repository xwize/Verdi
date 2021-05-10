#ifndef WORKSPACEWIDGET_H
#define WORKSPACEWIDGET_H

#include <QDockWidget>

namespace Ui {
class WorkspaceWidget;
}

class WorkspaceWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit WorkspaceWidget(QWidget *parent = 0);
    ~WorkspaceWidget();
    
public:
    void updateFromModel();
private:
    Ui::WorkspaceWidget *ui;
};

#endif // WORKSPACEWIDGET_H
