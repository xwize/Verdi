#include "displayglwidget.h"
#include "application.h"
#include "workspacevariables.h"
#include "scenewidget.h"

#include <QTimer>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>

DisplayGLWidget::DisplayGLWidget(QWidget *parent) :
    QGLWidget(parent)
{


    mpCamera = new Verdi::Camera();
    vApp->renderContext->setCamera(mpCamera);

    mCameraPosition = Verdi::Vec3(0.0f,0.0f,-10.0f);
    mCameraTarget = Verdi::Vec3(0.0f,0.0f,0.0f);

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(update()));
    timer->start(0);

    mUseERT = false;
    mUseESL = false;

    // Default to something reasonable
    for(int i = 0; i < FRAME_WINDOW_SIZE; ++i)
        mFrameDeltas[i] = 1.0f/60.0f;

    mFrameDeltaWritePosition = 0;

    mAppDeltaTime = 0.0f;
    mFrameDeltaTime = 0.0f;
    mFrameTimer.reset();
    mAppTimer.reset();

    vApp->sharedGLWidget = this;
}

void DisplayGLWidget::initializeGL()
{
    this->setAutoBufferSwap(false);

    setCameraPosition(Verdi::Vec3(-2,1,2));

    try {
        vApp->renderSystem->initialise(512,512);
    } catch(const Verdi::Exception& e) {

        QMessageBox msg;
        msg.setText( QString( Verdi::StringUtility::toString(e).c_str() ) );
        msg.exec();

        vApp->exit(1);
    }

     vApp->sceneWidget->updateCameraFromModel();
}

bool DisplayGLWidget::setupRender()
{
    vApp->renderContext->setDataSet(NULL);
    vApp->renderContext->setHardwareDataSet(NULL);
    vApp->renderContext->setTransferFunction(NULL);
    vApp->renderContext->setHardwareTransferFunction(NULL);
    vApp->renderContext->setEarlyRayTerminationEnabled(mUseERT);
    vApp->renderContext->setMinMaxOctree(NULL);

    if(mSelectedStrategyName.toStdString().empty())
        return false;

    if(mSelectedRenderableName.toStdString().empty())
        return false;
    else
    {
        Verdi::WorkspaceVariable* var = vApp->workspace->getVariable(
                    mSelectedRenderableName.toStdString() );

        if(var != NULL)
        {
            if(var->getType() == "DataSet")
            {
                DataSetVariable* dvar = (DataSetVariable*)var;
                vApp->renderContext->setDataSet( dvar->getDataSet() );

                if(dvar->getHardwareDataSet() == NULL) {
                    dvar->updateHardwareDataSet();
                    return false;
                }

                vApp->renderContext->setHardwareDataSet( dvar->getHardwareDataSet() );
                vApp->renderContext->setDimensionRatio( dvar->getDataSet()->getCorrectedDimensionRatio() );
                vApp->renderContext->setDimensions(
                            Verdi::Vec3(
                                dvar->getDataSet()->getWidth(),
                                dvar->getDataSet()->getHeight(),
                                dvar->getDataSet()->getDepth()));
            }
            else if(var->getType() == "Simulation")
            {
                SimulationVariable* svar = (SimulationVariable*)var;

                vApp->renderContext->setDimensionRatio( Verdi::Vec3(1.0f,1.0f,1.0f) );
                vApp->renderContext->setDimensions(
                            Verdi::Vec3(
                                svar->getWidth(),
                                svar->getHeight(),
                                svar->getDepth()));

                vApp->renderContext->setHardwareDataSet(svar->getHardwareDataSet());
            }
        }
    }

    Verdi::TransferFunction* tfunc = 0;
    Verdi::MinMaxOctree* octree = NULL;

    if(!mSelectedOctreeName.toStdString().empty() && mUseESL )
    {
        Verdi::WorkspaceVariable* var = vApp->workspace->getVariable(
                    mSelectedOctreeName.toStdString() );

        if(var != NULL && var->getType() == "MinMaxOctree") {
            MinMaxOctreeVariable* dvar = (MinMaxOctreeVariable*)var;
            octree = dvar->getMinMaxOctree();
            vApp->renderContext->setMinMaxOctree( dvar->getMinMaxOctree() );
        }
    }

    if(mSelectedTransferFunctionName.toStdString().empty())
        return false;
    else
    {
        Verdi::WorkspaceVariable* var = vApp->workspace->getVariable(
                    mSelectedTransferFunctionName.toStdString() );

        if(var != NULL && var->getType() == "TransferFunction") {
            TransferFunctionVariable* dvar = (TransferFunctionVariable*)var;
            tfunc = dvar->getTransferFunction();
            vApp->renderContext->setTransferFunction(  tfunc );

            if(dvar->needsUpdate() && octree != NULL)
                   octree->updateMappingTable(tfunc);

            if(dvar->getHardwareTransferFunction() == NULL)
                return false;

            vApp->renderContext->setHardwareTransferFunction( dvar->getHardwareTransferFunction() );
        }
    }

    vApp->renderContext->setCamera(mpCamera);
    vApp->renderSystem->useStrategy( mSelectedStrategyName.toStdString() );
    return true;
}

void DisplayGLWidget::printText()
{
    glViewport(0,0,width(),height());

    // Do the font rendering in pixel space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f,(double)width(),0.0f,(double)height(),-1.0f,1.0f);

    // Set identity matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Label the corners
    QFont font;
    float eps1 = 5.0f;
    font.setPointSize(8);
    glColor4f(1.0f,1.0f,1.0f,0.75f);

    float fps = 0.0f;
    if(mFrameDeltaTime != 0.0f)
        fps = 1.0f/mFrameDeltaTime;

    float nf = 0.0f;
    Verdi::Vec3 sm(0.0f,0.0f,0.0f);

    if(vApp->renderContext->getDataSet() != NULL) {
        nf = vApp->renderContext->getDataSet()->getNormalisationFactor();
        sm = vApp->renderContext->getDataSet()->getScaleMultiplier();
    }


    // Draw data set information
    std::stringstream str;
    str.precision(3);
    str << "Normalising Factor: " << std::fixed << nf;

    str.precision(4);
    str << "   Dimension Scaling: (" << std::fixed <<
                        sm.x << "," << sm.y << "," << sm.z << ")";

    glLoadIdentity();
    renderText(0.0f+eps1,0.0f+eps1, 0.0f, QString(str.str().c_str()),font);

    // Draw num iterations
    str.precision(2);
    str.clear();
    str.str("");

    str << "Iterations: " << std::fixed << vApp->renderContext->getNumIterations();
    glLoadIdentity();
    renderText(0.0f+eps1,height()-eps1-10.0f, 0.0f, QString(str.str().c_str()),font);

    // Draw frame rate
    str.precision(1);
    str.clear();
    str.str("");

    str << "Frame Rate: " << std::fixed << fps;
    glLoadIdentity();
    renderText(width()-eps1-95.0f,height()-eps1-10.0f, 0.0f,
               QString(str.str().c_str()),font);

}

void DisplayGLWidget::paintGL()
{

    this->makeCurrent();

    if(vApp->isTesting()) {
        mYaw = vApp->getUnitisedTestTime() * Verdi::Math::PI*2.0f;
        updateCamera();
    }

    if(setupRender() )
    {
        mFrameTimer.reset();
        vApp->renderSystem->beginFrame(mAppDeltaTime,0.0f);
        vApp->renderSystem->render();
        vApp->renderSystem->endFrame();
        printText();
        this->swapBuffers();
        mFrameDeltaTime = mFrameTimer.getSeconds();

        mFrameDeltas[mFrameDeltaWritePosition] = mFrameDeltaTime;
        mFrameDeltaWritePosition = (mFrameDeltaWritePosition + 1) % FRAME_WINDOW_SIZE;
    }
    else
    {
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        printText();
        this->swapBuffers();
    }
    VERDI_GL_OK();

    mAppDeltaTime = mAppTimer.getSeconds();
    mAppTimer.reset();

    // Compute averaged frame delta
    float avgDelta = 0.0f;

    for(int i = 0; i < FRAME_WINDOW_SIZE; ++i)
        avgDelta += mFrameDeltas[i];

    avgDelta /= (float)FRAME_WINDOW_SIZE;

    // Update the test
    vApp->tickTest(mAppDeltaTime,mFrameDeltaTime,avgDelta,mYaw);

    VERDI_GL_OK();
}

void DisplayGLWidget::resizeGL(int width, int height)
{
    vApp->renderSystem->resize(width,height);
    VERDI_GL_OK();
}

void DisplayGLWidget::mousePressEvent(QMouseEvent *event)
{
    mouseMoveEvent(event);
}

void DisplayGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint cursor = event->globalPos();

    if( (event->buttons() & Qt::LeftButton) ||
            (event->buttons() & Qt::RightButton) ||
            (event->buttons() & Qt::MiddleButton)) {

        this->setCursor(Qt::ClosedHandCursor);

        float dpitch = cursor.y() - mPrevCursor.y();
        float dyaw = cursor.x() - mPrevCursor.x();

        float d = 0.0025f;

        float eps = 0.0001f;
        float PI = Verdi::Math::PI;

        if( (event->buttons() & Qt::LeftButton) ||
                (event->buttons() & Qt::RightButton) )
        {
            mYaw	+= dyaw * d;
            mPitch	-= dpitch * d;
        }
        else
        {
            // Work out the right vector
            Verdi::Vec3 toTarget = mCameraTarget - mCameraPosition;
            Verdi::Vec3 up = Verdi::Vec3(0.0f,1.0f,0.0f);
            Verdi::Vec3 right = toTarget.cross(up) * -1.0f;
            right.normalise();

            Verdi::Vec3 offset = right * dyaw * d + up * dpitch * d;

            // Work out the new yaw
            setCameraTarget(mCameraTarget + offset);
            setCameraPosition(mCameraPosition + offset);
        }

        // Restrict the field of view to 90 degrees
        // up and down. We need epsilon to restrict
        // the view from reaching exactly 90 degrees
        // as this flips the view direction.

        if(mPitch < 0.0f + eps)
            mPitch = 0.0f + eps;

        if(mPitch > PI - eps)
            mPitch = PI - eps;

        updateCamera();
        vApp->sceneWidget->updateCameraFromModel();

        if ( (event->buttons() & Qt::RightButton) )
        {
            Verdi::Light l = vApp->renderContext->getLight();
            l.position = mCameraPosition;
            vApp->renderContext->setLight(l);
            vApp->sceneWidget->updateLightingFromModel();
        }
    }

    mPrevCursor = cursor;
}

void DisplayGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(Qt::OpenHandCursor);
    mPrevCursor = event->globalPos();
}

void DisplayGLWidget::updateCamera()
{
    Verdi::Vec3 pos = Verdi::Math::fromSpherical(mYaw,mPitch);

    mCameraPosition.x = pos.x * mZoom;
    mCameraPosition.y = pos.y * mZoom;
    mCameraPosition.z = pos.z * mZoom;

    mpCamera->setNearDistance(0.1f);
    mpCamera->setIdentity();
    mpCamera->lookAt(mCameraPosition,mCameraTarget,Verdi::Vec3(0.0f,1.0f,0.0f));
}

void DisplayGLWidget::setCameraPosition(Verdi::Vec3 pos)
{
    mZoom = pos.length();
    Verdi::Math::toSpherical(pos,&mYaw,&mPitch);
    updateCamera();
}

void DisplayGLWidget::setCameraTarget(Verdi::Vec3 target)
{
    mCameraTarget = target;
    updateCamera();
}

void DisplayGLWidget::wheelEvent(QWheelEvent *event)
{
    mZoom += ((float)event->delta())/500.0f;
    updateCamera();
    vApp->sceneWidget->updateCameraFromModel();
}
