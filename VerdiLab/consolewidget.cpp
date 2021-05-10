#include "consolewidget.h"
#include "ui_consolewidget.h"

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ConsoleWidget)
{
    setTitleBarWidget( new QWidget(parent) );
    ui->setupUi(this);
}

ConsoleWidget::~ConsoleWidget()
{
    delete ui;
}

void ConsoleWidget::executeCommand(QString string)
{
    ConsoleInputWidget* wid = (ConsoleInputWidget*)ui->textBrowser;

    QTextCursor cursor = wid->textCursor();

    // Delete the last line
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor,1);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor,1);
    cursor.removeSelectedText();

    // Insert the text
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor,1);
    cursor.insertText(">> " + string);

    wid->repaint();
    wid->executeCommand(string);
}
