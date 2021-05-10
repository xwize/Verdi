#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QDockWidget>
#include "transferfunctionglwidget.h"

namespace Ui {
class TransferFunctionWidget;
}

class TransferFunctionWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit TransferFunctionWidget(QWidget *parent = 0);
    ~TransferFunctionWidget();
    
    void updateFromModel();

    TransferFunctionGLWidget* getDisplayWidget();

private slots:
    void on_selectionCombo_currentIndexChanged(const QString &arg1);

    void on_channelCombo_currentIndexChanged(int index);

    void on_modeCombo_currentIndexChanged(int index);

    void on_brushSizeSlider_valueChanged(int value);

    void on_intensitySlider_valueChanged(int value);

    void on_clearChannelButton_clicked();

private:
    Ui::TransferFunctionWidget *ui;
};

#endif // TRANSFERFUNCTIONWIDGET_H
