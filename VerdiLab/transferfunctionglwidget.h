#ifndef TRANSFERFUNCTIONGLWIDGET_H
#define TRANSFERFUNCTIONGLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QTimer>

class TransferFunctionGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit TransferFunctionGLWidget(QObject *parent);
    
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void clearSelectedChannel();
    void splat(QMouseEvent* event);

    void setChannelMask(int mask) {
        mChannelMask = mask;
    }

    void setBrushSize(int size) {
        mBrushSize = size;
    }

    void setBrushIntensity(int intensity) {
        mBrushIntensity = intensity;
    }

    void setSelectedTransferFunctionName(QString func) {
        mSelectedTransferFunctionName = func;
    }

    QString getSelectedTransferFunctionName() {
        return mSelectedTransferFunctionName;
    }

    void setWriteMode(int mode) {
        mWriteMode = mode;
    }


    void mouseReleaseEvent(QMouseEvent *event);

private:
    int mChannelMask;
    int mBrushSize;
    int mBrushIntensity;
    int mWriteMode;
    float mSamplePosX;
    float mSamplePosY;
    float mZoom;
    float mPanX;
    float mPanY;
    QMouseEvent mPrevMouseEvent;
    QTimer mMouseButtonPressTimer;
    QTimer mFrameTimer;
    QPoint mPrevCursor;
    QString mSelectedTransferFunctionName;
signals:
    
public slots:
    
    void doSplat();
};

#endif // TRANSFERFUNCTIONGLWIDGET_H
