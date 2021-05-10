#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QDockWidget>
#include <QColorDialog>

namespace Ui {
class SceneWidget;
}

class SceneWidget : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit SceneWidget(QWidget *parent = 0);
    ~SceneWidget();
    
    void updateFromModel();
    void updateCameraFromModel();

    void updateLightingFromModel();

private slots:
    void on_compositingModeCombo_currentIndexChanged(const QString &arg1);

    void on_dataSetCombo_currentIndexChanged(const QString &arg1);

    void on_transferFunctionCombo_currentIndexChanged(const QString &arg1);

    void on_enableLightingCheck_clicked(bool checked);

    void on_ambientButton_clicked();

    void on_diffuseButton_clicked();

    void on_specularButton_clicked();

    void on_specularExponentSlider_valueChanged(int value);

    void on_planeCutCheck_toggled(bool checked);

    void on_cameraPosEdit_editingFinished();

    void on_cameraTargetEdit_editingFinished();

    void on_cameraFovEdit_editingFinished();

    void on_lightPositionEdit_editingFinished();

    void on_lightRadiusEdit_valueChanged(int value);

    void on_optimisationsCombo_currentIndexChanged(const QString &arg1);

private:

    Ui::SceneWidget *ui;
};

#endif // SCENEWIDGET_H
