
#include <GLee.h>
#include <GL/glu.h>

#include "VerdiDefaultRenderStrategy.h"
#include "VerdiException.h"
#include "VerdiCgUberShader.h"

namespace Verdi {

	DefaultRenderStrategy::DefaultRenderStrategy(const String& shaderFile) 
		: mShaderFile(shaderFile),  mIsInitialised(false)
	{
		mpShader = NULL;
	}

	void DefaultRenderStrategy::bindTarget(uint32_t fbo, uint32_t rbo, uint32_t ctex)
	{
		// Bind the frame-buffer object
		glBindFramebuffer(GL_FRAMEBUFFER,fbo);

		if(fbo != 0)
		{
			// Bind the texture attachment
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, ctex, 0);

			// Bind the depth/stencil attachment
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER,rbo);
		}
	}

	void DefaultRenderStrategy::submitFullscreenTexture(uint32_t textureId) const
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		// Set orthographic camera
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);

		// Set identity matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
		submitOrthoQuad(-1.0f,1.0f,1.0f,-1.0f);
		glDisable(GL_TEXTURE_2D);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		VERDI_GL_OK();
	}

	void DefaultRenderStrategy::unbindTarget()
	{
		bindTarget(0,0,0);
		VERDI_GL_OK();
	}

	void DefaultRenderStrategy::setupJitterTexture()
	{
		uint32_t dim = 128;
		unsigned short* data = new unsigned short[dim*dim];

		srand(25);

		for(uint32_t i = 0; i < dim*dim; ++i)
			data[i] = rand()%0xFFFF;

		glGenTextures(1,&mStochasticJitterTexture);
		glBindTexture(GL_TEXTURE_2D,mStochasticJitterTexture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
			dim, dim, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT, data);

		glBindTexture(GL_TEXTURE_2D,0);
		delete [] data;
	}

	void DefaultRenderStrategy::initialise(uint32_t width, uint32_t height) {

		if(mIsInitialised)
			shutdown();

		// Set the widths and heights
		mWidth = width;
		mHeight = height;

		assert(mpShader == NULL);

		// Call the child class
		_initialiseImpl(width,height);
		VERDI_GL_OK();

		// Initialisation done
		mIsInitialised = true;
	}

	void DefaultRenderStrategy::submitBox(const Vec3& offset, const Vec3& scale) const
	{
		Vec3 vertices[24] =
		{
			// Front Face
			Vec3(0.0f,1.0f,1.0f),Vec3(0.0f,0.0f,1.0f),
			Vec3(1.0f,0.0f,1.0f),Vec3(1.0f,1.0f,1.0f),

			// Back Face
			Vec3(0.0f,1.0f,0.0f),Vec3(1.0f,1.0f,0.0f),
			Vec3(1.0f,0.0f,0.0f),Vec3(0.0f,0.0f,0.0f),

			// Left Face
			Vec3(0.0f,1.0f,0.0f),Vec3(0.0f,0.0f,0.0f),
			Vec3(0.0f,0.0f,1.0f),Vec3(0.0f,1.0f,1.0f),

			// Right Face
			Vec3(1.0f,1.0f,0.0f),Vec3(1.0f,1.0f,1.0f),
			Vec3(1.0f,0.0f,1.0f),Vec3(1.0f,0.0f,0.0f),

			// Top Face
			Vec3(0.0f,1.0f,1.0f),Vec3(1.0f,1.0f,1.0f),
			Vec3(1.0f,1.0f,0.0f),Vec3(0.0f,1.0f,0.0f),

			// Bottom Face
			Vec3(0.0f,0.0f,1.0f),Vec3(0.0f,0.0f,0.0f),
			Vec3(1.0f,0.0f,0.0f),Vec3(1.0f,0.0f,1.0f)

		};

		// Submit the geometry
		glBegin(GL_QUADS);

		for(int i = 0; i < 24; ++i)
		{
			// Doing the transformation on the CPU
			// is silly but the bottleneck is the pixel shader
			// so it doesn't really matter all that much.
			Vec3 transformed = (vertices[i] * scale) + offset;
			glVertex3fv(transformed.values);
		}

		glEnd();
		VERDI_GL_OK();
	}

	void DefaultRenderStrategy::submitOrthoQuad(float left, float right, float top, float bottom, float d) const
	{
		// Reads pretty well with these variable
		// names...
		glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex3f(left,bottom,d);
		glTexCoord2i(1,0);
		glVertex3f(right,bottom,d);
		glTexCoord2i(1,1);
		glVertex3f(right,top,d);
		glTexCoord2i(0,1);
		glVertex3f(left,top,d);
		glEnd();
		VERDI_GL_OK();
	}

	void DefaultRenderStrategy::submitOrthoQuad(int left, int right, int top, int bottom, int d) const
	{
		// For extra precision
		glBegin(GL_QUADS);
		glTexCoord2i(0,0);
		glVertex3i(left,bottom,d);
		glTexCoord2i(1,0);
		glVertex3i(right,bottom,d);
		glTexCoord2i(1,1);
		glVertex3i(right,top,d);
		glTexCoord2i(0,1);
		glVertex3i(left,top,d);
		glEnd();
		VERDI_GL_OK();
	}

	void DefaultRenderStrategy::shutdown()
	{
		_shutdownImpl();
		VERDI_GL_OK();

		if(mpShader != NULL) {
			delete mpShader;
			mpShader = NULL;
		}

		// We need initialisation again
		mIsInitialised = false;
	}

	void DefaultRenderStrategy::contextChanged(const RenderContext* newContext)
	{
		// Change the pointer to the new context
		mpRenderContext = newContext;
	}

	void DefaultRenderStrategy::resize(uint32_t width, uint32_t height)
	{
		_resizeImpl(width,height);
		VERDI_GL_OK();
	}

	void DefaultRenderStrategy::suspend()
	{

	}

	void DefaultRenderStrategy::awake()
	{

	}

	void DefaultRenderStrategy::beginFrame(float dt, float time)
	{
		VERDI_GL_OK();

		// If the context isn't valid
		// freak out and make everything red
		if(!mpRenderContext->isValid() || !mIsInitialised || mpShader == NULL) {
			glClearColor(1.0f,0.0f,0.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			return;
		}

		_beginFrameImpl(dt,time);

		VERDI_GL_OK();

#ifdef _DEBUG
		glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif
	}

	void DefaultRenderStrategy::render()
	{
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		_renderImpl();
	}

	void DefaultRenderStrategy::endFrame()
	{
		_endFrameImpl();

#ifdef _DEBUG
		glPopAttrib();
#endif

		VERDI_GL_OK();
	}
}