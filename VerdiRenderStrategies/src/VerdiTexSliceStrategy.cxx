
#include <GLee.h>
#include <GL\GLU.h>

#include "VerdiTexSliceStrategy.h"
#include "VerdiCgUberShader.h"

namespace Verdi {

	TexSliceStrategy::TexSliceStrategy(const String& shaderPath) : DefaultRenderStrategy(shaderPath)
	{
		for(int i = 0; i < 2; ++i) {
			mFrameBufferObject[i] = 0;
			mFrameRenderBufferObject[i] = 0;
			mFinalRenderTexture[i] = 0;
		}
	}

	TexSliceStrategy::~TexSliceStrategy()
	{

	}

	void TexSliceStrategy::_getEffectDescriptorFromParameters(
		bool onTheFlyGradients, bool useLighting, 
		UberShader::EffectDescriptor& effectOut) const
	{
		StringVector defines;
		defines.clear();

		uint32_t id = 0;

		if(onTheFlyGradients) {
			defines.push_back("-DUSE_ON_THE_FLY_GRADIENTS=1");
			id |= 1;
		}

		if(useLighting) {
			defines.push_back("-DUSE_LIGHTING=1");
			id |= 2;
		}

		effectOut.identifier = id;
		effectOut.defines = defines;
	}

	void TexSliceStrategy::_setupShaders()
	{
		UberShader::EffectDescriptorVector effectDesc;
		effectDesc.clear();

		// Generate all possible combinations
		// of effect for the selected parameters
		for(int i = 0; i < 4; ++i)
		{
			CgUberShader::EffectDescriptor fxd;
			fxd.identifier = 0;
			fxd.defines.clear();

			_getEffectDescriptorFromParameters(
				(i & 0x01) > 0,
				(i & 0x02) > 0,
				fxd);

			effectDesc.push_back(fxd);
		}

		mpShader = new CgUberShader(mShaderFile,effectDesc);
		mpShader->load();
	}

	void TexSliceStrategy::_setupRenderTextures(GLuint width, GLuint height)
	{
		GLint internalFormat = GL_RGBA16F;
		GLenum format = GL_RGBA;
		GLenum type = GL_FLOAT;

		for(int i = 0; i < 2; ++i) {
			if(mFinalRenderTexture[i] != 0)
				glDeleteTextures(1,&mFinalRenderTexture[i]);

			if(mFrameBufferObject != 0)
				glDeleteFramebuffers(1,&mFrameBufferObject[i]);

			if(mFrameRenderBufferObject != 0)
				glDeleteRenderbuffers(1,&mFrameRenderBufferObject[i]);
		}

		// Generate the frame-buffer object
		glGenFramebuffers(2,mFrameBufferObject);

		// Texture for final
		glGenTextures(2,mFinalRenderTexture);

		// Render buffer for final
		glGenRenderbuffers(2,mFrameRenderBufferObject);

		// Bind to initialise
		for(int i = 0; i < 2; ++i) {
			glBindFramebuffer(GL_FRAMEBUFFER,mFrameBufferObject[i]);

			// Attach render-buffer
			glBindRenderbuffer(GL_RENDERBUFFER,mFrameRenderBufferObject[i]);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,width,height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER,mFrameRenderBufferObject[i]);

			// Create final texture
			glBindTexture(GL_TEXTURE_2D,mFinalRenderTexture[i]);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
				width, height, 0, format, type, NULL);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, mFinalRenderTexture[i], 0);

			// Check the status
			GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

			// If the hardware supports FBO's this should never be thrown
			if (status != GL_FRAMEBUFFER_COMPLETE)  
				VERDI_THROW(Verdi::HardwareFeatureUnsupportedException,
				"Failed to create framebuffer object. Get a better graphics card. Sorry.",0);
		}

		// Reset to the default frame-buffer
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// Reset to the default render-buffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Store the widths and heights
		// for later use
		mRenderTextureWidth = width;
		mRenderTextureHeight = height;
	}

	void TexSliceStrategy::_initialiseImpl(uint32_t width, uint32_t height)
	{
		VERDI_LOG("Setting up shaders...");
		_setupShaders();

		VERDI_LOG("Setting up jitter texture...");
		setupJitterTexture();

		VERDI_LOG("Setting up render targets...");
		_setupRenderTextures(mWidth,mHeight);
	}

	int TexSliceStrategy::_computeFrontIndex()
	{
		Vec3 cubeVerts[8] = {
			Vec3(-1,-1,-1),
			Vec3(1,-1,-1),
			Vec3(1,1,-1),
			Vec3(-1,1,-1),
			Vec3(-1,1,1),
			Vec3(1,1,1),
			Vec3(1,-1,1),
			Vec3(-1,-1,1)
		};

		float bestDist = Math::INFINITY_POSITIVE;
		int bestIndex = 0;

		Mat4 modelViewMat;
		Mat4 projectionMat;

		glGetFloatv(GL_MODELVIEW_MATRIX,modelViewMat.values);
		glGetFloatv(GL_PROJECTION_MATRIX,projectionMat.values);
	
		Mat4 combinedMat = modelViewMat * projectionMat;

		for(int i = 0; i < 8; ++i)
		{
			Vec3 camPos;
			camPos.x = mpRenderContext->getCamera()->getTransform()._31;
			camPos.y = mpRenderContext->getCamera()->getTransform()._32;
			camPos.z = mpRenderContext->getCamera()->getTransform()._33;
			camPos.normalise();

			Vec3 v = camPos - cubeVerts[i];

			if(v.lengthSquared() < bestDist)
			{
				bestDist = v.lengthSquared();
				bestIndex = i;
			}
		}

		return bestIndex;
	}

	void TexSliceStrategy::_submitPolygonSlice(int planeIndex)
	{
		glBegin(GL_POLYGON);
		for(int i = 0; i < 6; ++i)
			glVertex2i(i, planeIndex);
		glEnd();
	}

	void TexSliceStrategy::_submitBlockSlice(const Vec3& offset, const Vec3& scale, int planeIndex)
	{
		mpShader->setVectorParameter(mpShader->getSemanticParameter("Translation"),
			offset);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("Scale"),
			scale);

		_submitPolygonSlice(planeIndex);
	}

	void TexSliceStrategy::_beginFrameImpl(float dt, float time)
	{
		// Get the background colour
		Vec4 bg = mpRenderContext->getBackgroundColour();

		//------------------------------------------------------------------------------
		// Select the uber shader based on the render context parameters
		//------------------------------------------------------------------------------
		bool useLighting = mpRenderContext->getLightingEnabled();
		bool useOnTheFlyGradients = true;

		CgUberShader::EffectDescriptor fx;
		_getEffectDescriptorFromParameters(useOnTheFlyGradients,useLighting,fx);
		mpShader->useEffect(fx.identifier);

		//------------------------------------------------------------------------------
		// Setup frame parameters
		//------------------------------------------------------------------------------
		mpShader->setVectorParameter(mpShader->getSemanticParameter("Ambient"),
			mpRenderContext->getMaterial().ambient);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("Diffuse"),
			mpRenderContext->getMaterial().diffuse);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("Specular"),
			mpRenderContext->getMaterial().specular);

		mpShader->setScalarParameter(mpShader->getSemanticParameter("SpecularExponent"),
			mpRenderContext->getMaterial().specularExponent);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("LightPosition"),
			mpRenderContext->getLight().position);

		mpShader->setScalarParameter(mpShader->getSemanticParameter("LightRadius"),
			mpRenderContext->getLight().radius);

		mpShader->setScalarParameter(mpShader->getSemanticParameter("Time"),
			time);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("BackgroundColour"),
			bg);
	}

	void TexSliceStrategy::_renderImpl()
	{
		Verdi::Camera* cam = mpRenderContext->getCamera();

		// Set Camera
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(cam->getFieldOfView(),
			((float)mWidth/(float)mHeight),cam->getNearDistance(),cam->getFarDistance());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Load the inverted camera matrix
		// on the OpenGL matrix stack
		Mat4 invCamMat = cam->getTransform().getOrthonormalInverse();
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(invCamMat.values);

		Vec3 scale = mpRenderContext->getDimensionRatio();
		glScalef(scale.x,scale.y,scale.z);

		//------------------------------------------------------------------------------
		// Setup shader parameters
		//------------------------------------------------------------------------------
		if(mpRenderContext->getDataSet() != NULL) {

			Vec3 dim = Vec3(
				(float)mpRenderContext->getDataSet()->getWidth(),
				(float)mpRenderContext->getDataSet()->getHeight(),
				(float)mpRenderContext->getDataSet()->getDepth() );

			mpShader->setVectorParameter(mpShader->getSemanticParameter("DataSetDimensions"),
				dim);
		}

		mpShader->setTextureParameter(mpShader->getSemanticParameter("DataSet"),
			mpRenderContext->getHardwareDataSet()->getHardwareIdentifier());

		mpShader->setScalarParameter(mpShader->getSemanticParameter("NumIterations"),
			Math::clamp<int>((int)mpRenderContext->getNumIterations(),1,1000));

		mpShader->setTextureParameter(mpShader->getSemanticParameter("TransferFunction"),
			mpRenderContext->getHardwareTransferFunction()->getHardwareIdentifier());

		mpShader->setTextureParameter(mpShader->getSemanticParameter("JitterTex"),
			mStochasticJitterTexture);

		mpShader->setModelViewProjectionParameter(mpShader->getSemanticParameter("ModelViewProj"));	

		mpShader->setModelViewParameter(mpShader->getSemanticParameter("ModelView"));

		Verdi::Vec3 viewVec;
		viewVec.x = cam->getTransform()._31;
		viewVec.y = cam->getTransform()._32;
		viewVec.z = cam->getTransform()._33;
		viewVec = viewVec * -1.0f;
		viewVec.normalise();

		Verdi::Vec3 viewPos;
		viewPos.x = cam->getTransform()._41;
		viewPos.y = cam->getTransform()._42;
		viewPos.z = cam->getTransform()._43;

		mpShader->setVectorParameter(mpShader->getSemanticParameter("ViewVector"),
			viewVec);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("CameraPosition"),
			viewPos);

		mpShader->setVectorParameter(mpShader->getSemanticParameter("Translation"),
			Vec3(0.0f,0.0f,0.0f));

		int frontIndex = _computeFrontIndex();
		mpShader->setScalarParameter(mpShader->getSemanticParameter("FrontIndex"),frontIndex);

		float startDist = -1.732f;
		float stepDist = (2.0f*1.732f)/(float)mpRenderContext->getNumIterations();

		mpShader->setScalarParameter(mpShader->getSemanticParameter("PlaneStartDist"),
			startDist);

		mpShader->setScalarParameter(mpShader->getSemanticParameter("PlaneStepDist"),
			stepDist);

		//------------------------------------------------------------------------------
		// 0. Pass : Perform the texture slicing
		//------------------------------------------------------------------------------
		glViewport(0,0,mRenderTextureWidth,mRenderTextureHeight);

		bindTarget(mFrameBufferObject[0],
			mFrameRenderBufferObject[0],
			mFinalRenderTexture[0]);

		// Clear background colour
		Vec4 bg = mpRenderContext->getBackgroundColour();
		glClearColor(bg.x,bg.y,bg.z,bg.w);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Enable the shader and set the parameters
		mpShader->bind(0);

		glEnable(GL_BLEND);

		if( mpRenderContext->getCompositor().getCompositingMode() == Compositor::CCM_MIP) {
			// MIP
			glBlendEquation(GL_MAX);
			glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
		}
		else {
			/// Front to back blending
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
		}

		// Because we are rendering forward
		// we set depth func to greater
		glDepthFunc(GL_GREATER);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);

		const float ertThreshold = 0.95f;
		const int checkInterval = 40;
		bool useErt = mpRenderContext->getEarlyRayTerminationEnabled();

		int selectedRenderBuffer = 0;

		for(int i = 0; i < (int)mpRenderContext->getNumIterations(); ++i) {

			if( ((i+1) % checkInterval == 0) && useErt ) {

				mpShader->unbind();

				// Change the frame-buffer 
				bindTarget(mFrameBufferObject[1],
					mFrameRenderBufferObject[selectedRenderBuffer], 
					mFinalRenderTexture[1]);

				// Draw everything so far into this frame-buffer
				// really far away from the screen so that when the alpha test
				// passes, small values are written - meaning that
				// subsequent depth tests for these fragments will fail

				// Disable colour writes
				glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

				// Set identity matrix
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();

				// Set orthographic camera
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				//glOrtho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);

				// Use modelview matrix
				glMatrixMode(GL_MODELVIEW);

				// Use alpha testing 
				glAlphaFunc(GL_GREATER, ertThreshold);
				glEnable(GL_ALPHA_TEST);

				// Clear the depth buffer to the maximum
				// using the alpha-test to mask out
				// pixels that do not pass the ERT threshold
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,mFinalRenderTexture[0]);

				submitOrthoQuad(-1,1,1,-1,1);

				glEnable(GL_DEPTH_TEST);
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);

				// Disable alpha testing
				glDisable(GL_ALPHA_TEST);

				// Reset matrices
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();

				// Enable colour writes
				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

				bindTarget(mFrameBufferObject[0],
					mFrameRenderBufferObject[selectedRenderBuffer],
					mFinalRenderTexture[0]);

				mpShader->bind(0);
			}

			_submitPolygonSlice(i);
		}


		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		// Unbind shader
		mpShader->unbind();
	}

	void TexSliceStrategy::_endFrameImpl()
	{
		//------------------------------------------------------------------------------
		// Draw the final image to the screen.
		//------------------------------------------------------------------------------
		glViewport(0,0,mWidth,mHeight);
		unbindTarget();
		submitFullscreenTexture(mFinalRenderTexture[0]);
	}

	void TexSliceStrategy::_resizeImpl(uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;
	}

	void TexSliceStrategy::_shutdownImpl()
	{
		delete mpShader;
		mpShader = NULL;
	}
}