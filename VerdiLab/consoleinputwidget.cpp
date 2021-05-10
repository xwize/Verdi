#include "consoleinputwidget.h"
#include "workspacewidget.h"
#include "application.h"
#include "VerdiWorkspace.h"
#include "VerdiDefaultCommandParser.h"
#include <QKeyEvent>

ConsoleInputWidget::ConsoleInputWidget(QObject *parent) :
    QTextEdit((QWidget*)parent)
{
    mNextCommandIndex = 0;
    mCycleCommandIndex = 0;

    setText("");
    append(">> ");

    setUndoRedoEnabled(false);
    setContextMenuPolicy(Qt::NoContextMenu);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(restrictInput()));
}

QMenu* ConsoleInputWidget::createStandardContextMenu()
{
    restrictInput();
    return QTextEdit::createStandardContextMenu();
}

bool ConsoleInputWidget::cursorIsEditable(const QTextCursor& c)
{
    // Only restrict if we're not on the last line
    // The last line is defined from the last position
    // to the start, that begins with the console caret
    QTextCursor cursor = c;

    //cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor,1);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor,1);

    QString selText = cursor.selectedText();

    if(selText.size() < 3)
        return false;

    if(selText.remove(3,selText.size()) != ">> ") {
        return false;
    }

    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor,1);

    if( !cursor.atEnd() )
        return false;

    return true;
}

void ConsoleInputWidget::restrictInput()
{
    setUndoRedoEnabled(false);
    setReadOnly(true);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    if( cursorIsEditable(textCursor()) && !textCursor().hasSelection() ) {
        setReadOnly(false);
        setContextMenuPolicy(Qt::NoContextMenu);
    }
}

void ConsoleInputWidget::executeCommand(QString str)
{
    Verdi::WorkspaceCommand newCmd;

    Verdi::DefaultCommandParser parser;
    Verdi::WorkspaceCommandParser::ParseResult res = parser.parseCommandString(
        str.toStdString(), vApp->workspace, newCmd);

    bool commandOk = true;

    if(res.type != Verdi::WorkspaceCommandParser::ParseResult::RC_SUCCESS) {
        this->append("");
        this->setTextColor(QColor(127,0,0));
        this->append(QString(res.description.c_str()));
        commandOk = false;
    }

    if(commandOk) {
        this->append("");
        QString text = QString(vApp->workspace->executeCommand(newCmd).c_str());
        this->setTextColor(QColor(0,0,127));
        this->append(text);
        mPreviousCommands[mNextCommandIndex] = str;
        mNextCommandIndex = (mNextCommandIndex + 1) % MAX_PREV_COMMANDS;
        mCycleCommandIndex = mNextCommandIndex;
        vApp->updateWidgetsFromModel();
    }

    this->setTextColor(QColor(0,0,0));
    this->append("");
    this->append(">> ");
}

void ConsoleInputWidget::writeInput(QString string)
{
    QTextCursor cursor = textCursor();

    // Delete the last line
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor,1);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor,1);
    cursor.removeSelectedText();

    // Insert the text
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor,1);
    cursor.insertText(">> " + string);

    // Move to the end
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor, 1);
}

void ConsoleInputWidget::keyPressEvent(QKeyEvent *e)
{
    QTextCursor c = textCursor();

    if(!cursorIsEditable(c))
        return;

    if(e->key() == Qt::Key_Up) {

        int next = (mCycleCommandIndex - 1) % MAX_PREV_COMMANDS;
        if(!mPreviousCommands[next].isEmpty())
        {
            mCycleCommandIndex = next;
            writeInput(mPreviousCommands[mCycleCommandIndex]);
        }

        return;
    }

    if(e->key() == Qt::Key_Down) {
        int next = (mCycleCommandIndex + 1) % MAX_PREV_COMMANDS;
        if(!(mPreviousCommands[next].isEmpty() &&
                mPreviousCommands[mCycleCommandIndex].isEmpty()))
        {
            mCycleCommandIndex = next;
            writeInput(mPreviousCommands[mCycleCommandIndex]);
        }
    }

    if(e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Left) {

        QTextCursor cursor = textCursor();

        // Move to end of previous char
        cursor.movePosition(QTextCursor::PreviousCharacter,
                             QTextCursor::MoveAnchor,1);
        // Move to start
        cursor.movePosition(QTextCursor::StartOfLine,
                             QTextCursor::KeepAnchor,1);

        QString line = cursor.selectedText();
        if(line == ">>")
            return;
    }

    if(e->key() == Qt::Key_Tab)
    {
        // Auto complete... like I have the time!
        return;
    }

    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {

        QTextCursor cursor = textCursor();

        // Move to current char
        cursor.movePosition(QTextCursor::EndOfLine,
                             QTextCursor::MoveAnchor,0);
        // Move to start
        cursor.movePosition(QTextCursor::StartOfLine,
                             QTextCursor::KeepAnchor,1);

        QString str = cursor.selectedText();
        str = str.remove(0,3);

        if(!str.isEmpty()) {
            if(!str.endsWith(';'))
                str = str  + ";";

            this->executeCommand(str);
        }
        else {
            this->setTextColor(QColor(0,0,0));
            this->append("");
            this->append(">> ");
        }

        return;
    }

    QTextEdit::keyPressEvent(e);
}
