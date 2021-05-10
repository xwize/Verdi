#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T04:12:11
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

TARGET = VerdiLab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    workspacewidget.cpp \
    consolewidget.cpp \
    consoleinputwidget.cpp \
    displaywidget.cpp \
    transferfunctionwidget.cpp \
    transferfunctionglwidget.cpp \
    displayglwidget.cpp \
    workspacecommands.cpp \
    application.cpp \
    workspacevariables.cpp \
    scenewidget.cpp

HEADERS  += mainwindow.h \
    workspacewidget.h \
    consolewidget.h \
    consoleinputwidget.h \
    displaywidget.h \
    transferfunctionwidget.h \
    transferfunctionglwidget.h \
    displayglwidget.h \
    workspacecommands.h \
    application.h \
    workspacevariables.h \
    scenewidget.h

FORMS    += mainwindow.ui \
    workspacewidget.ui \
    consolewidget.ui \
    displaywidget.ui \
    transferfunctionwidget.ui \
    scenewidget.ui

INCLUDEPATH += "..\\VerdiMain\\include\\"

INCLUDEPATH += "..\\VerdiRenderStrategies\\include\\"

INCLUDEPATH += "..\\VerdiWorkspace\\include\\"

win32:LIBS += "..\\lib\\VerdiMain_debug.lib"

win32:LIBS += "..\\lib\\VerdiRenderStrategies_debug.lib"

win32:LIBS += "..\\lib\\VerdiWorkspace_debug.lib"

win32:LIBS += "..\\dependencies\\GL\\glut32.lib"

win32:LIBS += "..\\dependencies\\GLee\\Glee.lib"

win32:LIBS += "..\\dependencies\\Cg\\lib\\CgGL.lib"

win32:LIBS += "..\\dependencies\\Cg\\lib\\Cg.lib"

win32:LIBS += "..\\dependencies\\GL\\opengl32.lib"

win32:LIBS += "..\\dependencies\\GL\\glu32.lib"

win32:LIBS += "winmm.lib"

unix:LIBS += "../lib/libVerdiMain.a"

unix:LIBS += "../lib/libVerdiWorkspace.a"

unix:LIBS += "../lib/libVerdiRenderStrategies.a"

RC_FILE = app.rc

QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:libc

RESOURCES += \
    resedit.qrc
