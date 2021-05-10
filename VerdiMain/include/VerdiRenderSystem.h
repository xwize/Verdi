
#ifndef __Verdi_Render_System_H__
#define __Verdi_Render_System_H__

#include "VerdiPrerequisites.h"
#include "VerdiRenderStrategy.h"
#include "VerdiRenderContext.h"
#include "VerdiTimer.h"

namespace Verdi {

	/** Class responsible for controlling rendering. 
	@remarks
		The RenderSystem allows for the registration
		of instances of RenderStrategy (which represent
		the possible rendering algorithms that can be
		used by the system) along with RenderContext (the
		render state the algorithms require to operate).
		Methods for switching between contexts and
		strategies are provided.
	*/
	class RenderSystem
	{
	protected:
		int mWidth;
		int mHeight;
		typedef std::map<String,RenderStrategy*> RenderStrategyNameMap;
		RenderStrategyNameMap mRenderStrategies;
		RenderStrategy* mpCurrentStrategy;
		typedef std::map<String,RenderContext*> RenderContextNameMap;
		RenderContextNameMap mRenderContexts;
		RenderContext* mpCurrentContext;
		Timer mFrameTimer;
	public:

		/** Default constructor.
		*/
		RenderSystem();

		/** Default destructor.
		*/
		~RenderSystem();

		/** Returns the current RenderContext in use.
		*/
		RenderContext& getCurrentContext() const;

		/** Returns the current strategy name.
		*/
		String getCurrentStrategyName() const;

		/** Returns a reference to the current strategy in use.
		*/
		RenderStrategy& getCurrentStrategy() const;

		/** Returns the number of contexts registered.
		*/
		size_t getNumContexts() const { return mRenderContexts.size(); }

		/** Returns the number of strategies.
		*/
		size_t getNumStrategies() const { return mRenderStrategies.size(); }

		/** Initialise the render system.
		@remarks
			When initialise is called, all currently
			registered instances of RenderStrategy get their
			initialise method called.
			Proper usage must ensure that all required
			strategies are registered before hand.
		@param width The width reported to strategies.
		@param height The height reported to strategies.
		*/
		void initialise(int width, int height);

		/** Signify the start of a frame.
			This gives the render strategy a chance to
			setup the scene and prepare for rendering.
		*/
		void beginFrame(float dt, float time);

		/** Render using selected RenderStrategy.
			Internal times are used.
		*/
		void render();

		/** Notifies the render strategy to display the results
			to the screen.
		@returns Time between begin and end frame in seconds
		*/
		float endFrame();

		/** Request a resize of the viewport.
		*/
		void resize(uint32_t width, uint32_t height);

		/** Shutdown the renderer and all strategies.
		*/
		void shutdown();

		/** Returns the width.
		*/
		int getWidth() const { return mWidth; }

		/** Returns the height.
		*/
		int getHeight() const { return mHeight; }

		/** Register a render strategy with the RenderSystem.
		@remarks
			When registering a strategy, a name must be provided
			to uniquely identify the strategy instance.
		*/
		void registerStrategy(const String& name, RenderStrategy* strategy);

		/** Unregister a strategy.
		*/
		void unregisterStrategy(const String& name);

		/** Unregister all registered strategies.
		*/
		void unregisterAllStrategies();

		/** Returns a StringList of all registered strategies.
		*/
		StringVector getRegisteredStrategyNames() const;

		/** Use the strategy specified by name.
		@remarks
			When switching strategies, the previous strategy
			is notified by calling RenderStrategy::suspend
			and the new strategy is notified with a call to
			RenderStrategy::awake.
		*/
		void useStrategy(const String& name);

		/** Register a RenderContext instance with the system.
		*/
		void registerContext(const String& name, RenderContext* context);

		/** Unregister the context specified by name.
		*/
		void unregisterContext(const String& name);

		/** Unregister all registered contexts.
		*/
		void unregisterAllContexts();

		/** Returns a StringList of all registered contexts.
		*/
		StringVector getRegisteredContextNames() const;

		/** Use the context specified by name.
		@remarks
			The current RenderStrategy is notified of this change
			using the RenderStrategy::contextChanged method.
		*/
		void useContext(const String& name);

		/** Fires a context changed event.
		@remarks
			This allows a user to manually notify a render
			strategy of any changes made to the current render context,
			even though the pointer might not have changed.
		*/
		void fireContextChanged();
	};
}

#endif // __Verdi_Render_System_H__