#ifndef CONSOLEINPUTWIDGET_H
#define CONSOLEINPUTWIDGET_H

#include <QTextEdit>

class ConsoleInputWidget : public QTextEdit
{
    Q_OBJECT
private:
    static const int MAX_PREV_COMMANDS = 15;
    unsigned int mNextCommandIndex;
    unsigned int mCycleCommandIndex;
    QString mPreviousCommands[MAX_PREV_COMMANDS];
public:
    explicit ConsoleInputWidget(QObject *parent = 0);

    void keyPressEvent(QKeyEvent *e);
    void executeCommand(QString str);
    void writeInput(QString str);
    bool cursorIsEditable(const QTextCursor& c);
    QMenu* createStandardContextMenu();

signals:
public slots:
    void restrictInput();
};

#endif // CONSOLEINPUTWIDGET_H
