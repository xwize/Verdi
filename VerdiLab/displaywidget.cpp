#include "displaywidget.h"
#include "ui_displaywidget.h"
#include "displayglwidget.h"
#include "application.h"

DisplayWidget::DisplayWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::displaywidget)
{
    setTitleBarWidget( new QWidget(parent) );
    ui->setupUi(this);


}

DisplayWidget::~DisplayWidget()
{
    delete ui;
}

void DisplayWidget::updateFromModel()
{
   // Update strategies
   Verdi::StringVector names = vApp->renderSystem->getRegisteredStrategyNames();

   QString current = ui->strategyCombo->currentText();
   ui->strategyCombo->clear();

   for(int i = 0; i < names.size(); ++i) {
       ui->strategyCombo->addItem(QString(names[i].c_str()));
   }

   for(int i = 0; i < names.size(); ++i) {
       QString txt = ui->strategyCombo->itemText(i);
       if(txt == current) {
           ui->strategyCombo->setCurrentIndex(i);
           break;
       }
   }

   if(names.size() <= 1)
       ui->strategyCombo->setEnabled(false);

   DisplayGLWidget* gldisplay = (DisplayGLWidget*)ui->glDisplayWidget;
   gldisplay->setSelectedStrategyName(QString(ui->strategyCombo->currentText()));

   ui->iterationsSlider->setValue(vApp->renderContext->getNumIterations());
}

void DisplayWidget::on_strategyCombo_currentIndexChanged(int index)
{
    DisplayGLWidget* gldisplay = (DisplayGLWidget*)ui->glDisplayWidget;
    gldisplay->setSelectedStrategyName(QString(ui->strategyCombo->currentText()));
}

void DisplayWidget::on_iterationsSlider_valueChanged(int value)
{
    DisplayGLWidget* gldisplay = (DisplayGLWidget*)ui->glDisplayWidget;
    vApp->renderContext->setNumIterations(value);
}

/*void DisplayWidget::on_optimisationsCombo_currentIndexChanged(const QString &arg1)
{
    DisplayGLWidget* gldisplay = (DisplayGLWidget*)ui->glDisplayWidget;
    gldisplay->setUseERT(false);
    gldisplay->setUseESL(false);

    if(arg1 == "ERT") {
        gldisplay->setUseERT(true);
    }

    if(arg1 == "ESL")
        gldisplay->setUseESL(true);

    if(arg1 == "ERT + ESL")
    {
        gldisplay->setUseERT(true);
        gldisplay->setUseESL(true);
    }
}*/

DisplayGLWidget* DisplayWidget::getDisplayWidget()
{
    return (DisplayGLWidget*)ui->glDisplayWidget;
}
