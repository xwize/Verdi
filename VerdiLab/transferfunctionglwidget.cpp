#include "transferfunctionglwidget.h"
#include "application.h"
#include "workspacevariables.h"
#include "displaywidget.h"
#include "displayglwidget.h"
#include <QTimer>
#include <QMouseEvent>

#ifndef min
#define min(a,b) (a < b ? a : b)
#endif

#ifndef max
#define max(a,b) (a > b ? a : b)
#endif

TransferFunctionGLWidget::TransferFunctionGLWidget(QObject *parent) :
    QGLWidget((QWidget*)parent, vApp->sharedGLWidget ), mPrevMouseEvent(QEvent::MouseMove,
                                                                        QPoint(),
                                                                        Qt::NoButton,
                                                                        Qt::LeftButton,
                                                                        Qt::NoModifier)
{
    assert(vApp->sharedGLWidget != NULL);

    mBrushSize = 50;
    mBrushIntensity = 100;
    mChannelMask = 0x01;

    mSamplePosX = 0.0f;
    mSamplePosY = 0.0f;

    mPanX = 0.0f;
    mPanY = 0.0f;

    mZoom = 1.0f;

    connect(&mFrameTimer,SIGNAL(timeout()), this, SLOT(update()));
    connect(&mMouseButtonPressTimer,SIGNAL(timeout()),this,SLOT(doSplat()));

}

void TransferFunctionGLWidget::initializeGL()
{
    mFrameTimer.start(1000/30);
}

void TransferFunctionGLWidget::paintGL()
{
    //this->makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT);

    float fWindowAspect = (float)width()/(float)height();

    // Set orthographic camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-fWindowAspect*mZoom + mPanX,
            fWindowAspect*mZoom + mPanX,
            -1.0f*mZoom + mPanY,
            1.0f*mZoom + mPanY,
            -1.0f,1.0f);

    // Set identity matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int numX = 12;
    int numY = 12;

    // Clear the colour buffer
    // with a checker pattern
    for(int i = 0; i < numX; ++i)
    {
        for(int j = 0; j < numY; ++j)
        {
            if( (i+j)%2 == 0)
                glColor3f(0.15f,0.15f,0.15f);
            else
                glColor3f(0.0f,0.0f,0.0f);

            float x0 = (float)i/(float)numX;
            float x1 = (float)(i+1)/(float)numX;

            float y0 = (float)j/(float)numY;
            float y1 = (float)(j+1)/(float)numY;

            x0 = (x0-0.5f)*2.0f;
            x1 = (x1-0.5f)*2.0f;

            y0 = (y0-0.5f)*2.0f;
            y1 = (y1-0.5f)*2.0f;

            // Draw quad
            glBegin(GL_QUADS);
            // Bottom left
            glVertex3f(x0,y0,0.0f);
            // Bottom right
            glVertex3f(x1,y0,0.0f);
            // Top right
            glVertex3f(x1,y1,0.0f);
            // Top left
            glVertex3f(x0,y1,0.0f);
            glEnd();
        }
    }


    Verdi::HardwareTransferFunction* htf = NULL;
    TransferFunctionVariable* tfv = (TransferFunctionVariable*)vApp->workspace->getVariable(
                mSelectedTransferFunctionName.toStdString());

    if(tfv != NULL) {

        if(tfv->needsUpdate())
            tfv->updateHardwareTransferFunction();

        htf = tfv->getHardwareTransferFunction();

        if(htf != NULL) {

            // Use the texture to draw
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D,htf->getHardwareIdentifier());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

            glColor4f(1.0f,1.0f,1.0f,1.0f);
            // Draw full-screen quad
            glBegin(GL_QUADS);
            // Bottom left
            glTexCoord2f(0.0f,1.0f);
            glVertex3f(-1.0f,-1.0f,0.0f);
            // Bottom right
            glTexCoord2f(1.0f,1.0f);
            glVertex3f(1.0f,-1.0f,0.0f);
            // Top right
            glTexCoord2f(1.0f,0.0f);
            glVertex3f(1.0f,1.0f,0.0f);
            // Top left
            glTexCoord2f(0.0f,0.0f);
            glVertex3f(-1.0f,1.0f,0.0f);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
        }
    }

    float size = (float)(mBrushSize)/1024.0f;
    float xmin = -1.0f + (mSamplePosX) * 2.0f - size;
    float xmax = -1.0f + (mSamplePosX) * 2.0f + size;

    glBegin(GL_LINES);
    glVertex2f(xmin,-1.0f);
    glVertex2f(xmin,1.0f);
    glVertex2f(xmax,-1.0f);
    glVertex2f(xmax,1.0f);
    glEnd();

    // Do the font rendering in pixel space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f,(double)width(),0.0f,(double)height(),-1.0f,1.0f);

    // Set identity matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Label the corners
    QFont font;
    font.setPointSize(8);
    float eps1 = 5.0f;
    glColor4f(1.0f,1.0f,1.0f,0.75f);

    float alphaVal = 0.0f;

    if(tfv != NULL) {
        alphaVal = tfv->getTransferFunction()->sample(mSamplePosX,mSamplePosY).w;
    }

    std::stringstream str;
    str.precision(4);
    str << "Sample Position: (" << std::fixed << mSamplePosX << "," << mSamplePosY << ") ";
    str << "   Alpha: " << std::fixed << alphaVal;

    glLoadIdentity();
    renderText(0.0f+eps1,0.0f+eps1,0.0f, QString(str.str().c_str()),font);

    VERDI_GL_OK();
}

void TransferFunctionGLWidget::clearSelectedChannel()
{
    TransferFunctionVariable* tfv = (TransferFunctionVariable*)vApp->workspace->getVariable(
                mSelectedTransferFunctionName.toStdString());

    if(tfv == NULL)
        return;

    //Verdi::Vec4* data = tfv->getTransferFunction()->getData();

    int width = tfv->getTransferFunction()->getWidth();
    int height = tfv->getTransferFunction()->getHeight();

    float clearValue = Verdi::Math::clamp((float)(mBrushIntensity) / 255.0f,0.0f,1.0f);

    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {

            Verdi::Vec4 col = tfv->getTransferFunction()->getPixel(i,j);

            for(int k = 0; k < 4; ++k) {
                if(mChannelMask & (1 << k)) {
                    float* val = ((float*)&col.x) + k; //((float*)&data[i + j * width]) + k;
                    (*val) = clearValue;
                }
            }

            tfv->getTransferFunction()->setPixel(i,j,col);
        }
    }

    tfv->markForUpdate();
}
void TransferFunctionGLWidget::wheelEvent(QWheelEvent *event)
{
    mZoom += (float)(event->delta())*0.001f;
    mZoom = max(1.0f/16.0f,mZoom);
    mZoom = min(mZoom,1.0f/0.5f);
}

void TransferFunctionGLWidget::mousePressEvent(QMouseEvent *event)
{
    mPrevMouseEvent = QMouseEvent(event->type(),event->pos(),event->button(),event->buttons(),event->modifiers());
    mMouseButtonPressTimer.start(1000/12);
}

void TransferFunctionGLWidget::doSplat()
{
    splat(&mPrevMouseEvent);
}

void TransferFunctionGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mPrevMouseEvent = QMouseEvent(event->type(),event->pos(),event->button(),event->buttons(),event->modifiers());
    mMouseButtonPressTimer.stop();
}

void TransferFunctionGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    float rx = (float)event->x()/((float)this->geometry().width() - 1);
    float ry = (float)event->y()/((float)this->geometry().height() - 1);

    rx = -1.0f + rx * 2.0f;
    ry = -1.0f + ry * 2.0f;

    float fWindowAspect = (float)width()/(float)height();
    rx *= (fWindowAspect * mZoom);
    ry *= mZoom;

    rx += mPanX;
    ry += mPanY;

    rx = 0.5f + rx * 0.5f;
    ry = 0.5f + ry * 0.5f;

    rx = Verdi::Math::clamp(rx,0.0f,1.0f);
    ry = Verdi::Math::clamp(ry,0.0f,1.0f);

    mSamplePosX = rx;
    mSamplePosY = ry;

    {
        if(! (event->buttons() & Qt::MiddleButton) )
          this->setCursor(Qt::CrossCursor);
    }

    mPrevCursor = mPrevMouseEvent.globalPos();
    mPrevMouseEvent = QMouseEvent(event->type(),event->pos(),event->button(),event->buttons(),event->modifiers());
}

void TransferFunctionGLWidget::splat(QMouseEvent *event)
{
    TransferFunctionVariable* tfv = (TransferFunctionVariable*)vApp->workspace->getVariable(
                mSelectedTransferFunctionName.toStdString());

    if(tfv == NULL)
    {
        printf("No transfer function selected.\n");
        return;
    }

    // Compute intensity
    float scaleFactor = (float)(mBrushIntensity) / 255.0f;
    scaleFactor = powf(scaleFactor,2.0f);

    float dt = vApp->displayWidget->getDisplayWidget()->getAppDeltaTime();
    scaleFactor *= dt * 8.0f;

    // Compute sizes
    Verdi::TransferFunction::Window wnd;

    int tfwidth = tfv->getTransferFunction()->getWidth();
    int tfheight = tfv->getTransferFunction()->getHeight();

    float size = (float)(mBrushSize)/1024.0f;
    float halfSize = size * 0.5;

    int sx = (mSamplePosX - halfSize) * (tfwidth - 1);
    int sy = (mSamplePosY - halfSize) * (tfheight - 1);

    int ex = (mSamplePosX + halfSize) * (tfwidth - 1);
    int ey = (mSamplePosY + halfSize) * (tfheight - 1);

    wnd.x = sx;
    wnd.width = (ex - sx) + 1;

    wnd.y = sy;
    wnd.height = (ey - sy) + 1;

    if( event->buttons() & Qt::MiddleButton )
    {
        float dx = event->globalPos().x() - mPrevCursor.x();
        float dy = event->globalPos().y() - mPrevCursor.y();

        dx *= -0.0025f;
        dy *= 0.0025f;

        mPanX = min(max(mPanX+dx,-1.0f),1.0f);
        mPanY = min(max(mPanY+dy,-1.0f),1.0f);

        this->setCursor(Qt::ClosedHandCursor);
    }


    if( !(event->buttons() & Qt::LeftButton) && !((event->buttons() & Qt::RightButton)))
        return;

    // Compute write mode
    Verdi::TransferFunction::WriteMode mode =
            mWriteMode != 0 ? Verdi::TransferFunction::TFWM_SUB : Verdi::TransferFunction::TFWM_ADD;

    if( (event->buttons() & Qt::RightButton) ) {

        if(mode == Verdi::TransferFunction::TFWM_SUB)
            mode = Verdi::TransferFunction::TFWM_ADD;
        else if(mode == Verdi::TransferFunction::TFWM_ADD)
             mode = Verdi::TransferFunction::TFWM_SUB;
    }

    tfv->getTransferFunction()->gaussianSplat(wnd,mChannelMask,scaleFactor,0.707,mode);
    tfv->updateHardwareTransferFunction();
}

void TransferFunctionGLWidget::resizeGL(int width, int height)
{
    glViewport(0,0,width,height);
}
