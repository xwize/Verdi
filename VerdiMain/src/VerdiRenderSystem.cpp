
#include "VerdiRenderSystem.h"
#include "VerdiException.h"

// We also need GL, GLU and GLee.
#include <GLee.h>

namespace Verdi {

	RenderSystem::RenderSystem() : mpCurrentStrategy(NULL), mpCurrentContext(NULL)
	{
		mRenderStrategies.clear();
		mRenderContexts.clear();
	}

	RenderSystem::~RenderSystem()
	{
		VERDI_LOG("Destructor called.");

		shutdown();

		RenderStrategyNameMap::iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i) {
			delete i->second;
		}

		unregisterAllContexts();
		unregisterAllStrategies();
	}

	StringVector RenderSystem::getRegisteredStrategyNames() const
	{
		StringVector res;
		RenderStrategyNameMap::const_iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i) {
			res.push_back(i->first);
		}
		return res;
	}

	String RenderSystem::getCurrentStrategyName() const
	{
		assert(mpCurrentStrategy != NULL);

		RenderStrategyNameMap::const_iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i) {
			if(i->second == mpCurrentStrategy)
				return i->first;
		}

		return "";
	}

	void RenderSystem::initialise(int width, int height)
	{
		mWidth = width;
		mHeight = height;

		VERDI_LOG("Initialising render system.");

		RenderStrategyNameMap::iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i)
		{
			if( !i->second->isInitialised() ) {
				VERDI_LOG(i->first << " is not initialised. Initialising...");
				i->second->initialise(mWidth,mHeight);
			}
		}

		VERDI_GL_OK();
	}

	void RenderSystem::resize(uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;

		VERDI_LOG("Resizing render system.");

		RenderStrategyNameMap::iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i)
		{
			if( i->second->isInitialised() )
				i->second->resize(width,height);
		}

		VERDI_GL_OK();
	}

	void RenderSystem::shutdown()
	{
		RenderStrategyNameMap::iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i)
		{
			i->second->shutdown();
		}

		VERDI_GL_OK();
	}

	void RenderSystem::beginFrame(float dt, float time)
	{
		mFrameTimer.reset();
		if(mpCurrentStrategy != NULL)
		{
			mpCurrentStrategy->beginFrame(dt,time);
		}
	}

	void RenderSystem::render()
	{
		if(mpCurrentStrategy != NULL)
		{
			mpCurrentStrategy->render();
			VERDI_GL_OK();
		}
	}

	float RenderSystem::endFrame()
	{
		if(mpCurrentStrategy != NULL)
		{
			mpCurrentStrategy->endFrame();
		}

		return (float)mFrameTimer.getSeconds();
	}

	void RenderSystem::registerStrategy(const String& name, RenderStrategy* strategy)
	{
		if( mRenderStrategies.find(name) != mRenderStrategies.end() )
			VERDI_THROW(Verdi::ObjectDuplicateFoundException,"Strategy already exists with name " + name,0);
		
		mRenderStrategies[name] = strategy;
	}

	void RenderSystem::unregisterAllStrategies()
	{
		mRenderStrategies.clear();
	}

	void RenderSystem::unregisterStrategy(const String& name)
	{
		RenderStrategyNameMap::iterator i = mRenderStrategies.find(name);
		if(i != mRenderStrategies.end())
		{
			mRenderStrategies.erase(i);
		}
	}

	void RenderSystem::useStrategy(const String& name)
	{
		assert( mRenderStrategies.find(name) != mRenderStrategies.end() );
		
		RenderStrategy* previousStrategy = mpCurrentStrategy;
		if(previousStrategy != NULL)
			previousStrategy->suspend();

		mpCurrentStrategy = mRenderStrategies[name];
		mpCurrentStrategy->awake();
	}
	
	void RenderSystem::registerContext(const String& name, RenderContext* context)
	{
		if( mRenderContexts.find(name) != mRenderContexts.end() )
			VERDI_THROW(Verdi::ObjectDuplicateFoundException,"Context already exists with name " + name,0);

		mRenderContexts[name] = context;
	}

	void RenderSystem::unregisterAllContexts()
	{
		mRenderContexts.clear();
	}

	void RenderSystem::useContext(const String& name)
	{
		assert( mRenderContexts.find(name) != mRenderContexts.end() );
		VERDI_LOG("Selecting context " << name << ".");
		mpCurrentContext = mRenderContexts[name];
		fireContextChanged();
	}

	void RenderSystem::fireContextChanged()
	{
		RenderStrategyNameMap::iterator i = mRenderStrategies.begin();
		for( ; i != mRenderStrategies.end(); ++i)
		{
			i->second->contextChanged(mpCurrentContext);
		}
	}

}