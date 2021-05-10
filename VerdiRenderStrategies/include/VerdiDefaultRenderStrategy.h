
#ifndef __Verdi_Default_Strategy_H__
#define __Verdi_Default_Strategy_H__

#include "VerdiRenderStrategy.h"
#include "VerdiUberShader.h"

namespace Verdi {

	/** A base class for the two default render strategies.
		Seeing as both strategies use CG for shaders and share
		some code, it makes sense to write it once and put it here.
	*/
	class DefaultRenderStrategy : public RenderStrategy
	{
	protected:
		// Some helper functionality
		void submitOrthoQuad(float left, float right, float top, float bottom, float d = 0.0f) const;
		void submitOrthoQuad(int left, int right, int top, int bottom, int d) const;
		void submitBox(const Vec3& pos, const Vec3& dim) const;
		void submitFullscreenTexture(uint32_t textureId) const;

		// For working with render targets / frame-buffer objects
		void bindTarget(uint32_t fbo, uint32_t rbo, uint32_t ctex);
		void unbindTarget();

		// For creating jitter texture
		void setupJitterTexture();

	protected:
		UberShader* mpShader;
		const RenderContext* mpRenderContext;
		String mShaderFile;
		bool mIsInitialised;
		uint32_t mStochasticJitterTexture;
		uint32_t mWidth;
		uint32_t mHeight;
	protected:
		// Methods that need to be implemented
		virtual void _initialiseImpl(uint32_t w, uint32_t h) = 0;
		virtual void _beginFrameImpl(float dt, float time) = 0;
		virtual void _renderImpl() = 0;
		virtual void _endFrameImpl() = 0;
		virtual void _shutdownImpl() = 0;
		virtual void _resizeImpl(uint32_t width, uint32_t height) = 0;
	private:
		/** Private default constructor.
			Makes no sense to instantiate this class
			without first providing the shader path.
		*/
		DefaultRenderStrategy() {}
	public:

		/** Parameterised constructor.
		*/
		DefaultRenderStrategy(const String& shaderFile);

		// Interface implementation
		virtual void initialise(uint32_t width, uint32_t height);

		// Begin a frame.
		virtual void beginFrame(float dt, float time);
		
		// Perform rendering
		virtual void render();

		// End a frame.
		virtual void endFrame();

		// Does some shutdown and calls shutdownImpl
		virtual void shutdown();

		// Context changed
		virtual void contextChanged(const RenderContext* newContext);

		// Suspend implementation
		virtual void suspend();

		// Awake implementation
		virtual void awake();

		// Resize implementation
		virtual void resize(uint32_t width, uint32_t height);

		// Return the initialised flag
		virtual bool isInitialised() const { return mIsInitialised; }
	};
}

#endif // __Verdi_Default_Strategy_H__