#ifndef DISPLAYGLWIDGET_H
#define DISPLAYGLWIDGET_H

#include <QGLWidget>
#include "application.h"

class DisplayGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit DisplayGLWidget(QWidget *parent = 0);
    
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    bool setupRender();

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void updateCamera();
    void printText();

    Verdi::Vec3 getCameraPosition() {
        return mCameraPosition;
    }

    Verdi::Vec3 getCameraTarget() {
        return mCameraTarget;
    }

    void setCameraPosition(Verdi::Vec3 pos);

    void setCameraTarget(Verdi::Vec3 target);

    void setSelectedOctreeName(QString octree) {
        mSelectedOctreeName = octree;
    }

    void setSelectedStrategyName(QString strategy)  {
        mSelectedStrategyName = strategy;
    }

    void setSelectedDataSetName(QString dataSet) {
        mSelectedRenderableName = dataSet;
    }

    QString getSelectedDataSetName() {
        return mSelectedRenderableName;
    }

    float getAppDeltaTime() {
        return mAppDeltaTime;
    }

    void setSelectedTransferFunctionName(QString tfunc) {
        mSelectedTransferFunctionName = tfunc;
    }

    QString getSelectedTransferFunctionName() {
        return mSelectedTransferFunctionName;
    }

    void setUseERT(bool useERT) {
        mUseERT = useERT;
    }

    void setUseESL(bool useESL) {
        mUseESL = useESL;
    }

    void mousePressEvent(QMouseEvent *event);
private:
    Verdi::Camera* mpCamera;
    QString mSelectedStrategyName;
    QString mSelectedRenderableName;
    QString mSelectedTransferFunctionName;
    QString mSelectedOctreeName;

    bool mUseERT;
    bool mUseESL;

    float mYaw;
    float mPitch;
    float mZoom;

    static const int FRAME_WINDOW_SIZE = 10;
    float mFrameDeltas[FRAME_WINDOW_SIZE];
    int mFrameDeltaWritePosition;

    float mFrameDeltaTime;
    float mAppDeltaTime;

    QPoint mPrevCursor;

    Verdi::Timer mFrameTimer;
    Verdi::Timer mAppTimer;

    Verdi::Vec3 mCameraPosition;
    Verdi::Vec3 mCameraTarget;

signals:
    
public slots:
    
};

#endif // DISPLAYGLWIDGET_H
