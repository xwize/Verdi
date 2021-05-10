#include "scenewidget.h"
#include "ui_scenewidget.h"
#include "displaywidget.h"
#include "displayglwidget.h"
#include "transferfunctionwidget.h"
#include "transferfunctionglwidget.h"
#include "application.h"



SceneWidget::SceneWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SceneWidget)
{
    setTitleBarWidget( new QWidget(parent) );
    ui->setupUi(this);

    on_optimisationsCombo_currentIndexChanged( ui->optimisationsCombo->currentText() );
}

SceneWidget::~SceneWidget()
{
    delete ui;
}

void SceneWidget::updateFromModel()
{
    // Set all parameters from render context
    const Verdi::WorkspaceVariableNameMap& vars = vApp->workspace->getVariables();

    ui->transferFunctionCombo->clear();
    ui->dataSetCombo->clear();

    int dsc = 0;
    int tfc = 0;

    Verdi::WorkspaceVariableNameMap::const_iterator i = vars.begin();
    for( ; i != vars.end(); i++) {

        if(i->second->getType() == "TransferFunction") {
            ui->transferFunctionCombo->addItem(QString(i->first.c_str()));
            tfc++;
        }

        if(i->second->getType() == "DataSet") {
            ui->dataSetCombo->addItem(QString(i->first.c_str()));
            dsc++;
        }
    }

    ui->transferFunctionCombo->setDisabled(tfc == 0);
    ui->dataSetCombo->setDisabled(dsc == 0);

    vApp->displayWidget->getDisplayWidget()->setSelectedTransferFunctionName(
                ui->transferFunctionCombo->currentText());

    vApp->displayWidget->getDisplayWidget()->setSelectedDataSetName(
                ui->dataSetCombo->currentText());

    // Update compostor combo
    ui->compositingModeCombo->setCurrentIndex(0);
    if( vApp->renderContext->getCompositor().getCompositingMode() !=
            Verdi::Compositor::CCM_EMISSION_ABSORPTION) {
        ui->compositingModeCombo->setCurrentIndex(1);
    }

    // Set all parameters from render context
    {
        ui->octreeCombo->clear();

        int osc = 0;

        Verdi::WorkspaceVariableNameMap::const_iterator i = vars.begin();
        for( ; i != vars.end(); i++) {

            if(i->second->getType() == "MinMaxOctree") {
                ui->octreeCombo->addItem(QString(i->first.c_str()));
                osc++;
            }
        }

        ui->octreeCombo->setEnabled(osc != 0);

        vApp->displayWidget->getDisplayWidget()->setSelectedOctreeName(
                    ui->octreeCombo->currentText());
    }

    // Update everything else
    updateCameraFromModel();

    updateLightingFromModel();
}

void SceneWidget::updateCameraFromModel()
{
    Verdi::Camera* cam = vApp->renderContext->getCamera();

    if(cam == NULL)
        return;

    QString fovText = QString( Verdi::StringUtility::toString(cam->getFieldOfView()).c_str() );
    ui->cameraFovEdit->setText(fovText);

    Verdi::Vec3 camPos = vApp->displayWidget->getDisplayWidget()->getCameraPosition();
    Verdi::Vec3 camTarget = vApp->displayWidget->getDisplayWidget()->getCameraTarget();

    QString posText = QString( Verdi::StringUtility::toString(camPos).c_str() );
    QString tarText = QString( Verdi::StringUtility::toString(camTarget).c_str() );

    ui->cameraPosEdit->setText(posText);
    ui->cameraTargetEdit->setText(tarText);
}

void SceneWidget::updateLightingFromModel()
{
    bool useLighting = vApp->renderContext->getLightingEnabled();

    ui->enableLightingCheck->setChecked( useLighting );

    Verdi::Vec3 lightPos = vApp->renderContext->getLight().position;
    QString posText = QString( Verdi::StringUtility::toString(lightPos).c_str() );

    float lightRadius = vApp->renderContext->getLight().radius;

    ui->lightPositionEdit->setText(posText);
    ui->lightRadiusEdit->setValue(lightRadius/0.1f);

    ui->specularExponentSlider->setValue(
                vApp->renderContext->getMaterial().specularExponent);

    ui->ambientButton->setDisabled( !useLighting );
    ui->diffuseButton->setDisabled( !useLighting );
    ui->specularButton->setDisabled( !useLighting );
    ui->lightPositionEdit->setDisabled( !useLighting );
    ui->specularExponentSlider->setDisabled( !useLighting );
    ui->lightRadiusEdit->setDisabled( !useLighting );
}

void SceneWidget::on_compositingModeCombo_currentIndexChanged(const QString &arg1)
{
    vApp->renderContext->setCompositor(
                Verdi::Compositor(Verdi::Compositor::CCM_EMISSION_ABSORPTION));

    if(arg1 == "MIP")
        vApp->renderContext->setCompositor(
                    Verdi::Compositor(Verdi::Compositor::CCM_MIP));
}

void SceneWidget::on_dataSetCombo_currentIndexChanged(const QString &arg1)
{
    vApp->displayWidget->getDisplayWidget()->setSelectedDataSetName(
                ui->dataSetCombo->currentText());
}

void SceneWidget::on_transferFunctionCombo_currentIndexChanged(const QString &arg1)
{
    vApp->displayWidget->getDisplayWidget()->setSelectedTransferFunctionName(
                ui->transferFunctionCombo->currentText());

    vApp->transferFunctionWidget->getDisplayWidget()->setSelectedTransferFunctionName(
                ui->transferFunctionCombo->currentText());
}

void SceneWidget::on_cameraPosEdit_editingFinished()
{
    QString arg1 = ui->cameraPosEdit->text();
    Verdi::Vec3 pos = Verdi::StringUtility::toVec3(arg1.toStdString());
    vApp->displayWidget->getDisplayWidget()->setCameraPosition(pos);
    updateCameraFromModel();
}

void SceneWidget::on_enableLightingCheck_clicked(bool checked)
{
    vApp->renderContext->setLightingEnabled(checked);
    updateLightingFromModel();
}

void SceneWidget::on_ambientButton_clicked()
{
    Verdi::Material m = vApp->renderContext->getMaterial();
    Verdi::Vec4 a = m.ambient;
    QColor prevCol = QColor(a.x*255.0f,a.y*255.0f,a.z*255.0f,a.w*255.0f);
    QColor col = QColorDialog::getColor(prevCol,NULL,QString(),QColorDialog::ShowAlphaChannel);
    if(col.isValid()) {
        m.ambient = Verdi::Vec4(col.red()/255.0f,
                                col.green()/255.0f,
                                col.blue()/255.0f,
                                col.alpha()/255.0f);

        vApp->renderContext->setMaterial(m);
    }
}

void SceneWidget::on_diffuseButton_clicked()
{
    Verdi::Material m = vApp->renderContext->getMaterial();
    Verdi::Vec4 a = m.diffuse;
    QColor prevCol = QColor(a.x*255.0f,a.y*255.0f,a.z*255.0f,a.w*255.0f);
    QColor col = QColorDialog::getColor(prevCol,NULL,QString(),QColorDialog::ShowAlphaChannel);
    if(col.isValid()) {
        m.diffuse = Verdi::Vec4(col.red()/255.0f,
                                col.green()/255.0f,
                                col.blue()/255.0f,
                                col.alpha()/255.0f);

        vApp->renderContext->setMaterial(m);
    }
}

void SceneWidget::on_specularButton_clicked()
{
    Verdi::Material m = vApp->renderContext->getMaterial();
    Verdi::Vec4 a = m.specular;
    QColor prevCol = QColor(a.x*255.0f,a.y*255.0f,a.z*255.0f,a.w*255.0f);
    QColor col = QColorDialog::getColor(prevCol,NULL,QString(),QColorDialog::ShowAlphaChannel);
    if(col.isValid()) {
        m.specular = Verdi::Vec4(col.red()/255.0f,
                                col.green()/255.0f,
                                col.blue()/255.0f,
                                col.alpha()/255.0f);

        vApp->renderContext->setMaterial(m);
    }
}

void SceneWidget::on_specularExponentSlider_valueChanged(int value)
{
    Verdi::Material m = vApp->renderContext->getMaterial();
    m.specularExponent = (float)value;
    vApp->renderContext->setMaterial(m);
}

void SceneWidget::on_planeCutCheck_toggled(bool checked)
{

}



void SceneWidget::on_cameraTargetEdit_editingFinished()
{
    QString arg1 = ui->cameraTargetEdit->text();
    Verdi::Vec3 pos = Verdi::StringUtility::toVec3(arg1.toStdString());
    vApp->displayWidget->getDisplayWidget()->setCameraTarget(pos);
    updateCameraFromModel();
}

void SceneWidget::on_cameraFovEdit_editingFinished()
{
    QString arg1 = ui->cameraFovEdit->text();
    Verdi::Degree fov = Verdi::StringUtility::toFloat(arg1.toStdString());
    vApp->renderContext->getCamera()->setFieldOfView(fov);
    updateCameraFromModel();
}

void SceneWidget::on_lightPositionEdit_editingFinished()
{
    QString arg1 = ui->lightPositionEdit->text();
    Verdi::Vec3 pos = Verdi::StringUtility::toVec3(arg1.toStdString());
    Verdi::Light light = vApp->renderContext->getLight();
    light.position = pos;
    vApp->renderContext->setLight(light);
}

void SceneWidget::on_lightRadiusEdit_valueChanged(int value)
{
    Verdi::Light light = vApp->renderContext->getLight();
    light.radius = (float)value*0.1f;
    vApp->renderContext->setLight(light);
}

void SceneWidget::on_optimisationsCombo_currentIndexChanged(const QString &arg1)
{
    DisplayGLWidget* gldisplay = (DisplayGLWidget*)vApp->displayWidget->getDisplayWidget();
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
}
