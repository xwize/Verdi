
#include <GLee.h>
#include <GL/glu.h>

#include "VerdiRayCastStrategy.h"
#include "VerdiCgUberShader.h"

namespace Verdi {

	RayCastStrategy::RayCastStrategy(const String& shaderFile) : DefaultRenderStrategy(shaderFile)
	{
		mFinalRenderTexture = 0;
		mFrameBufferObject = 0;

		// Set the textures to NULL
		for(int i = 0; i < 2; ++i ) {
			mFaceRenderTexture[i] = 0;
			mFaceRenderBufferObject[i] = 0;
		}
	}

	RayCastStrategy::~RayCastStrategy()
	{
	}

	void RayCastStrategy::_submitNearPlane()
	{
		submitOrthoQuad(-1.0f,1.0f,1.0f,-1.0f);
	}

	void RayCastStrategy::_submitProxyGeometry(bool onlyHalf)
	{

		if(mpRenderContext->getMinMaxOctree() != NULL)
		{
			_recurseMinMaxOctree(mpRenderContext->getMinMaxOctree()->getRootNode(), onlyHalf);
		}
		else
		{
			submitBox(Vec3(-1.0f,-1.0f,-1.0f),Vec3(2.0f,2.0f,2.0f));
		}
	}

	void RayCastStrategy::_drawMinMaxOctreeNode( MinMaxOctree::Node* node )
	{
		Vec3 offset;
		Vec3 scale;

		MinMaxOctree::Node* root = mpRenderContext->getMinMaxOctree()->getRootNode();
		offset.x = -1.0f + 2.0f*((float)node->window.x / (float)root->window.width);
		offset.y = -1.0f + 2.0f*((float)node->window.y / (float)root->window.height);
		offset.z = -1.0f + 2.0f*((float)node->window.z / (float)root->window.depth);

		scale.x = (float)node->window.width / (float)root->window.width;
		scale.y = (float)node->window.height / (float)root->window.height;
		scale.z = (float)node->window.depth / (float)root->window.depth;

		scale *= 2.0f;

		submitBox(offset,scale);
	}

	bool RayCastStrategy::_recurseMinMaxOctree( MinMaxOctree::Node* parent, bool onlyHalf )
	{
		if(parent->isLeaf) {
			return ( mpRenderContext->getMinMaxOctree()->isVisible(parent) );
		}
		else {

			if(!mpRenderContext->getMinMaxOctree()->isVisible(parent))
				return false;

			bool optimise = true;
			bool visible[8];

			for(int i = 0; i < 8; ++i) {
				bool vis = _recurseMinMaxOctree(parent->children[i],false);
				optimise = optimise && vis;
				visible[i] = vis;
			}

			if(optimise && parent != mpRenderContext->getMinMaxOctree()->getRootNode())
				return optimise;

			for(int i = 0; i < 8; ++i) {
				if(visible[i])
					_drawMinMaxOctreeNode(parent->children[i]);
			}

			return false;
		}
	}

	void RayCastStrategy::_setupRenderTextures(GLuint width, GLuint height)
	{
		GLint internalFormat = GL_RGBA16F;
		GLenum format = GL_RGBA;
		GLenum type = GL_FLOAT;

		// Generate the frame-buffer object
		if(mFrameBufferObject == 0)
			glGenFramebuffers(1,&mFrameBufferObject);

		// Ensure that if we have created 
		// previous render textures, we destroy
		// and recreate them
		for(int i = 0; i < 2; ++i) {
			if(mFaceRenderTexture[i] != 0)
				glDeleteTextures(1,&mFaceRenderTexture[i]);

			if(mFaceRenderBufferObject != 0)
				glDeleteRenderbuffers(1,&mFaceRenderBufferObject[i]);
		}

		if(mFinalRenderTexture != 0)
			glDeleteTextures(1,&mFinalRenderTexture);

		// Texture for back faces
		glGenTextures(1,&mFaceRenderTexture[0]);

		// Texture for front faces
		glGenTextures(1,&mFaceRenderTexture[1]);

		// Texture for final
		glGenTextures(1,&mFinalRenderTexture);

		VERDI_GL_OK();

		// Create a render buffer for depth test
		glGenRenderbuffers(1,&mFaceRenderBufferObject[0]);

		// Create a render buffer for front faces
		glGenRenderbuffers(1,&mFaceRenderBufferObject[1]);

		// Bind to initialise
		glBindFramebuffer(GL_FRAMEBUFFER,mFrameBufferObject);

		VERDI_GL_OK();

		// Attach render buffer
		for(int i = 0; i < 2; ++i) {
			glBindRenderbuffer(GL_RENDERBUFFER,mFaceRenderBufferObject[i]);
			VERDI_GL_OK();

			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,width,height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER,mFaceRenderBufferObject[i]);
			VERDI_GL_OK();
		}

		// Attach colour textures
		for(int i = 0; i < 2; ++i) {
			glBindTexture(GL_TEXTURE_2D,mFaceRenderTexture[i]);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
				width, height, 0, format, type, NULL);

			VERDI_GL_OK();

			// Bind the render textures to see if they will work...
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				 GL_TEXTURE_2D, mFaceRenderTexture[i], 0);

			VERDI_GL_OK();

			// Check the status
			GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

			// If the hardware supports FBO's this should never be thrown
			if (status != GL_FRAMEBUFFER_COMPLETE)  
			{
				String error = "Unknown";
				switch(status)
				{
				case GL_FRAMEBUFFER_UNDEFINED : error = "GL_FRAMEBUFFER_UNDEFINED"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT : error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER : error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER : error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
				case GL_FRAMEBUFFER_UNSUPPORTED : error = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE : error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
				case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB : error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
				default : VERDI_GL_OK(); break;
				};

				VERDI_THROW(Verdi::HardwareFeatureUnsupportedException,
				"Failed to create framebuffer object. Get a better graphics card. Sorry. Reason: " + error,0);
			}
		}

		// Create final texture
		glBindTexture(GL_TEXTURE_2D,mFinalRenderTexture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			width, height, 0, format, type, NULL);

		VERDI_GL_OK();

		// Reset to the default frame-buffer
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// Reset to the default render-buffer
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Store the widths and heights
		// for later use
		mRenderTextureWidth = width;
		mRenderTextureHeight = height;

		VERDI_GL_OK();
	}

	void RayCastStrategy::_getEffectDescriptorFromParameters(
		bool onTheFlyGradients, bool useErt, bool useLighting,  bool useLMIP,
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

		if(useErt) {
			defines.push_back("-DUSE_ERT=1");
			id |= 4;
		}

		if(useLMIP) {
			defines.push_back("-DUSE_LMIP=1");
			id |= 8;
		}

		effectOut.identifier = id;
		effectOut.defines = defines;
	}

	void RayCastStrategy::_setupShaders()
	{
		UberShader::EffectDescriptorVector effectDesc;
		effectDesc.clear();

		// Generate all possible combinations
		// of effect for the selected parameters
		for(int i = 0; i < 16; ++i)
		{
			CgUberShader::EffectDescriptor fxd;
			fxd.identifier = 0;
			fxd.defines.clear();

			_getEffectDescriptorFromParameters(
				(i & 0x01) > 0,
				(i & 0x02) > 0,
				(i & 0x04) > 0,
				(i & 0x08) > 0, fxd);

			effectDesc.push_back(fxd);
		}

		mpShader = new CgUberShader(mShaderFile,effectDesc);
		mpShader->load();
	}

	void RayCastStrategy::_initialiseImpl(uint32_t width, uint32_t height)
	{
		VERDI_LOG("Setting up shaders...");
		_setupShaders();
		VERDI_GL_OK();

		VERDI_LOG("Setting up jitter texture...");
		setupJitterTexture();
		VERDI_GL_OK();

		VERDI_LOG("Setting up render targets...");
		_setupRenderTextures(mWidth,mHeight);
		VERDI_GL_OK();
	}

	void RayCastStrategy::_beginFrameImpl(float dt, float time)
	{
		// Get the background colour
		Vec4 bg = mpRenderContext->getBackgroundColour();

		//------------------------------------------------------------------------------
		// Select the uber shader based on the render context parameters
		//------------------------------------------------------------------------------
		bool useMIP = mpRenderContext->getCompositor().getCompositingMode() == Compositor::CCM_MIP;
		bool useERT = mpRenderContext->getEarlyRayTerminationEnabled();
		bool useLighting = mpRenderContext->getLightingEnabled();
		bool useOnTheFlyGradients = true;

		CgUberShader::EffectDescriptor fx;
		_getEffectDescriptorFromParameters(useOnTheFlyGradients,useERT,useLighting,useMIP,fx);
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

		VERDI_GL_OK();

	}

	void RayCastStrategy::_renderImpl()
	{
		// Get the background colour
		Vec4 bg = mpRenderContext->getBackgroundColour();

		Verdi::Camera* cam = mpRenderContext->getCamera();

		// Nice rendering
		glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

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

		VERDI_GL_OK();

		//------------------------------------------------------------------------------
		// Setup render specific shader parameters
		//------------------------------------------------------------------------------
		Vec3 dim = mpRenderContext->getDimensions();

		mpShader->setVectorParameter(mpShader->getSemanticParameter("DataSetDimensions"),
			dim);

		mpShader->setTextureParameter(mpShader->getSemanticParameter("BackFaceTex"),
			mFaceRenderTexture[0]);

		mpShader->setTextureParameter(mpShader->getSemanticParameter("FrontFaceTex"),
			mFaceRenderTexture[1]);

		mpShader->setTextureParameter(mpShader->getSemanticParameter("JitterTex"),
			mStochasticJitterTexture);

		mpShader->setTextureParameter(mpShader->getSemanticParameter("DataSet"),
			mpRenderContext->getHardwareDataSet()->getHardwareIdentifier());

		mpShader->setScalarParameter(mpShader->getSemanticParameter("NumIterations"),
			Math::clamp<int>((int)mpRenderContext->getNumIterations(),1,1000));

		mpShader->setTextureParameter(mpShader->getSemanticParameter("TransferFunction"),
			mpRenderContext->getHardwareTransferFunction()->getHardwareIdentifier());

		mpShader->setModelViewProjectionParameter(mpShader->getSemanticParameter("ModelViewProj"));	

		mpShader->setModelViewParameter(mpShader->getSemanticParameter("ModelView"));

		//------------------------------------------------------------------------------
		// 0. Pass: Draw back faces into a render texture.
		//------------------------------------------------------------------------------
		// Bind position shaders
		mpShader->bind(0);

		// Draw back faces into back face depth and colour targets
		glViewport(0,0,mRenderTextureWidth,mRenderTextureHeight);
		bindTarget(mFrameBufferObject, mFaceRenderBufferObject[BACK_FACE_TEXTURE],
			mFaceRenderTexture[BACK_FACE_TEXTURE]);

		// Set the depth testing mode to greater
		// and clear the depth buffer to zero
		// we can now draw the back-most faces of any proxy geometry
		// we might need.
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClearDepth(0.0f);
		glDepthFunc(GL_GEQUAL);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);
		
		_submitProxyGeometry();
		VERDI_GL_OK();

		//------------------------------------------------------------------------------
		// 1. Pass: Draw front faces reusing depth info from back face pass.
		//------------------------------------------------------------------------------
		// Draw front faces
		glViewport(0,0,mRenderTextureWidth,mRenderTextureHeight);
		bindTarget(mFrameBufferObject, mFaceRenderBufferObject[FRONT_FACE_TEXTURE],
			mFaceRenderTexture[FRONT_FACE_TEXTURE]);
		VERDI_GL_OK();

		// Clear the depth buffer to 1
		// to draw the front-faces
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClearDepth(1.0f);
		glClearStencil(0);
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		VERDI_GL_OK();

		// Draw all back faces with depth testing off
		// add to stencil buffer
		// 
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
		glStencilFunc(GL_ALWAYS,0,0xFF);
		_submitProxyGeometry();
		VERDI_GL_OK();

		// Draw all front faces with depth testing off
		// decr stencil buffer
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
		glStencilFunc(GL_ALWAYS,0,0xFF);
		_submitProxyGeometry();
		VERDI_GL_OK();

		// Draw front faces with depth testing on
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDisable(GL_STENCIL_TEST);
		glStencilMask(0);
		_submitProxyGeometry();
		VERDI_GL_OK();

		// Draw the near plane depth testing off
		// check stencil buf for > 0
		// We need to bind the near plane shader
		mpShader->bind(1);
		glDisable(GL_DEPTH_TEST);
		glStencilMask(0);
		glDepthMask(GL_FALSE);
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		glDisable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		glStencilFunc(GL_NOTEQUAL,0,0xFF);
		_submitNearPlane();

		// Unbind position shader
		mpShader->unbind();
		VERDI_GL_OK();

		//------------------------------------------------------------------------------
		// 2. Pass: Perform the ray casting using the information from last two passes.
		//------------------------------------------------------------------------------
		glViewport(0,0,mRenderTextureWidth,mRenderTextureHeight);
		bindTarget(mFrameBufferObject,mFaceRenderBufferObject[BACK_FACE_TEXTURE],mFinalRenderTexture);

		// Disable stencil test
		glDisable(GL_STENCIL_TEST);

		// We only want back face pixels to cast on
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		// Enable depth testing
		// Reuse the depth buffer values from previous passes for fast rejection
		// and to stop z-fighting along shared face boundaries.
		glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);

		// Clear the colour buffer to the background colour
		glClearColor(bg.x,bg.y,bg.z,bg.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use ray-cast shader
		mpShader->bind(RAY_CASTING_PASS);

		// Submit proxy geometry
		_submitProxyGeometry();
		VERDI_GL_OK();

		//Disable all shaders
		mpShader->unbind();

		// Draw wireframe if requested
		glViewport(0,0,mRenderTextureWidth,mRenderTextureHeight);
		bindTarget(mFrameBufferObject,mFaceRenderBufferObject[1],mFinalRenderTexture);

		bool showWireframe = mpRenderContext->getMinMaxOctree() != NULL;
		if(showWireframe) {
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glLineWidth(2.0f);
			glColor4f(1.0f,1.0f,1.0f,0.75f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			_submitProxyGeometry(true);
			glLineWidth(1.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			VERDI_GL_OK();
		}

		VERDI_GL_OK();
	}

	void RayCastStrategy::_endFrameImpl()
	{
		//------------------------------------------------------------------------------
		// Draw the final image to the screen. 
		//------------------------------------------------------------------------------
		// Don't want render textures
		glViewport(0,0,mWidth,mHeight);
		unbindTarget();
		VERDI_GL_OK();

		// Clear background colour
		Vec4 bg = mpRenderContext->getBackgroundColour();
		glClearColor(bg.x,bg.y,bg.z,bg.w);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Disable depth testing and culling
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		// Use the texture to draw
		submitFullscreenTexture(mFinalRenderTexture);
		VERDI_GL_OK();

		// Disable everything
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	}

	void RayCastStrategy::_resizeImpl(uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;
		VERDI_GL_OK();
	}

	void RayCastStrategy::_shutdownImpl()
	{
		VERDI_GL_OK();
	}
}
