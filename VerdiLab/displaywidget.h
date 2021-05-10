#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QDockWidget>
#include "displayglwidget.h"

namespace Ui {
class displaywidget;
}

class DisplayWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit DisplayWidget(QWidget *parent = 0);
    ~DisplayWidget();
    
    void updateFromModel();

    DisplayGLWidget* getDisplayWidget();

private slots:
    void on_strategyCombo_currentIndexChanged(int index);

    void on_iterationsSlider_valueChanged(int value);

private:
    QString mSelectedStrategyName;
    Ui::displaywidget *ui;


};

#endif // DISPLAYWIDGET_H
