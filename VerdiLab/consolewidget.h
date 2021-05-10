#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include "application.h"
#include "consoleinputwidget.h"

#include <QDockWidget>

namespace Ui {
class ConsoleWidget;
}

class ConsoleWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit ConsoleWidget(QWidget *parent = 0);
    ~ConsoleWidget();

    void executeCommand(QString string);

private:
    Ui::ConsoleWidget *ui;
};

#endif // CONSOLEWIDGET_H
