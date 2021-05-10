
#include "VerdiRenderContext.h"

namespace Verdi {

	RenderContext::RenderContext(HardwareDataSet* dataSet, Camera* camera)
		: mpCamera(camera), mpHardwareDataSet(dataSet), mNumIterations(64),
			mBackgroundColour(0.0f,0.0f,0.0f,0.0),
			mCompositor(Compositor::CCM_EMISSION_ABSORPTION),
			mDimensionRatio(1.0f,1.0f,1.0f), mDataMode(RCDM_STATIC)
	{
		mpTransferFunction = NULL;
		mpDataSet = NULL;
		mpHardwareTransferFunction = NULL;
		mpMinMaxOctree = NULL;
		mUseLighting = true;
		mUseERT = true;
	}

	RenderContext::~RenderContext()
	{
		VERDI_LOG("Destructor called.");
	}

	bool RenderContext::isValid() const
	{
		if(mpHardwareDataSet == NULL ||
			mpHardwareTransferFunction == NULL || mpCamera == NULL)
			return false;

		return true;
	}
}