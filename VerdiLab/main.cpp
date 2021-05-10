#include <QtGui/QApplication>
#include "mainwindow.h"
#include "application.h"

#include <QPlastiqueStyle>
#include <QCleanlooksStyle>

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    Application::setStyle(new QCleanlooksStyle );

    //Application::setStyle(new QPlastiqueStyle );

    //QPalette p(QColor(100,100,100),QColor(110,110,110));
    //p.setColor(QPalette::Button,QColor(100,100,100));
   // p.setColor(QPalette::Base,QColor(255,255,255));
   // p.setColor(QPalette::Highlight,QColor(247,147,30));
   // p.setColor(QPalette::Text,QColor(20,20,20));
    //p.setColor(QPalette::Window, QColor(100,100,100));

   // a.setPalette(p);

    MainWindow w;
    w.show();

    return a.exec();
}
