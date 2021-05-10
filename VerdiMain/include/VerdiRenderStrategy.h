
#ifndef __Verdi_Render_Strategy_H__
#define __Verdi_Render_Strategy_H__

#include "VerdiPrerequisites.h"
#include "VerdiRenderContext.h"

namespace Verdi {

	/** This is an interface that all rendering algorithms
		must implement to be used within the framework.
	*/
	class RenderStrategy
	{
	protected:
	public:

		/** Initialise the render strategy.
		@remarks
			This is called when the user initiates RenderSystem::initialise.
			All one-time initialisation must be performed within this method.
			The initial width and height are provided, but implementations
			should also account for modification through the resize method.
		*/
		virtual void initialise(uint32_t width, uint32_t height) = 0;

		/** Start a frame.
		@remarks
			This is called prior to rendering operations for a render
			strategy to set up any per-frame parameters.
		*/
		virtual void beginFrame(float dt, float time) = 0;

		/** Perform the actual rendering.
		@remarks
			This begin frame,render,and end frame methods
			have been written to be flexible. In future,
			this method may take in a block size, to render
			blocks of data at a time.
		*/
		virtual void render() = 0;

		/** End a frame.
		@remarks 
			This is called at the end of a frame to notify
			a render strategy to end-rendering and display its
			results to the screen.
		*/
		virtual void endFrame() = 0;

		/** Shutdown the strategy.
		@remarks
			This gives the strategy an opportunity to clean-up any data
			allocated during execution. It is called when the user initiates
			RenderSystem::shutdown.
		*/
		virtual void shutdown() = 0;

		/** Notify of a context change.
		@remarks
			When the user decides to switch to another context a 
			render strategy may need to recreate some objects or 
			perform some computation. This method provides an opportunity
			for implementations to do this.
		*/
		virtual void contextChanged(const RenderContext* newContext) = 0;

		/** Notify when a render strategy has changed (from this).
		@remarks
			When the user decides to switch render strategy, the
			previous render strategy is notified through this method.
			*/
		virtual void suspend() = 0;

		/** Notify when a render strategy has changed (to this).
		@remarks
			When a render strategy is switched to this strategy it
			is notified through this method.
		*/
		virtual void awake() = 0;

		/** Notify the render strategy of a resize.
		*/
		virtual void resize(uint32_t width, uint32_t height) = 0;

		/** Must return true after initialisation.
		*/
		virtual bool isInitialised() const = 0;
	};
}

#endif // __Verdi_Render_Strategy_H__