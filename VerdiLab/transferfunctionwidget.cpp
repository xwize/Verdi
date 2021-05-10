#include "transferfunctionwidget.h"
#include "ui_transferfunctionwidget.h"
#include "transferfunctionglwidget.h"
#include "application.h"
#include "displaywidget.h"

TransferFunctionWidget::TransferFunctionWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TransferFunctionWidget)
{
    setTitleBarWidget( new QWidget(parent) );
    ui->setupUi(this);

    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;

    tfglwidget->setBrushSize( ui->brushSizeSlider->value() );
    tfglwidget->setBrushIntensity( ui->intensitySlider->value() );
    tfglwidget->setWriteMode( ui->modeCombo->currentIndex() );

    int mask = 1 << ui->channelCombo->currentIndex();

    // For all channels
    if(ui->channelCombo->currentIndex() == 4)
        mask = (1 << 4) - 1;

    tfglwidget->setChannelMask(mask);
}

TransferFunctionWidget::~TransferFunctionWidget()
{
    delete ui;
}

void TransferFunctionWidget::updateFromModel()
{
    const Verdi::WorkspaceVariableNameMap& vars = vApp->workspace->getVariables();
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;

    int idx = 0;
    int c = 0;

    /*Verdi::WorkspaceVariableNameMap::const_iterator i = vars.begin();
    for( ; i != vars.end(); i++) {

        if(i->second->getType() == "TransferFunction") {
            ui->selectionCombo->addItem(QString(i->first.c_str()));

            if(i->first == tfglwidget->getSelectedTransferFunction().toStdString())
                idx = c;

            c++;
        }
    }*/

    if(!ui->glDisplayWidget->getSelectedTransferFunctionName().isEmpty())
        c = 1;


    //ui->selectionCombo->setDisabled( c == 0 );
    //ui->openButton->setDisabled(c == 0 );
    //ui->saveButton->setDisabled( c == 0 );
    //ui->preIntegrateButton->setDisabled( c == 0 );
    ui->clearChannelButton->setDisabled( c == 0 );
    ui->channelCombo->setDisabled( c == 0 );
    ui->brushSizeSlider->setDisabled( c == 0 );
    ui->modeCombo->setDisabled( c == 0 );
    ui->intensitySlider->setDisabled( c == 0 );

    //ui->selectionCombo->setCurrentIndex(idx);
    //tfglwidget->setSelectedTransferFunction( ui->selectionCombo->currentText() );
}

TransferFunctionGLWidget *TransferFunctionWidget::getDisplayWidget()
{
    return ui->glDisplayWidget;
}

void TransferFunctionWidget::on_selectionCombo_currentIndexChanged(const QString &arg1)
{
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;
    tfglwidget->setSelectedTransferFunctionName(arg1);
}

void TransferFunctionWidget::on_channelCombo_currentIndexChanged(int index)
{
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;

    int mask = 1 << index;

    // For all channels
    if(index == 4)
        mask = (1 << 4) - 1;

    tfglwidget->setChannelMask(mask);
}

void TransferFunctionWidget::on_modeCombo_currentIndexChanged(int index)
{
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;
    tfglwidget->setWriteMode(index);
}

void TransferFunctionWidget::on_brushSizeSlider_valueChanged(int value)
{
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;
    tfglwidget->setBrushSize(value);
}

void TransferFunctionWidget::on_intensitySlider_valueChanged(int value)
{
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;
    tfglwidget->setBrushIntensity(value);
}

void TransferFunctionWidget::on_clearChannelButton_clicked()
{
    TransferFunctionGLWidget* tfglwidget = (TransferFunctionGLWidget*)ui->glDisplayWidget;
    tfglwidget->clearSelectedChannel();
}
