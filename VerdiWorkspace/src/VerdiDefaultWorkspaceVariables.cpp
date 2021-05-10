
#include "VerdiDefaultWorkspaceVariables.h"
#include "Verdi.h"

namespace Verdi {

	DataSetVariable::DataSetVariable(Verdi::DataSet* dataSet, const Verdi::String& file)
	{
		mpDataSet = dataSet;
		mpHardwareDataSet = NULL;
		mFileName = file;

		if(!mFileName.empty())
			mIsFromFile = true;
	}

	DataSetVariable::~DataSetVariable()
	{
		delete mpHardwareDataSet;
		delete mpDataSet;
	}

	void DataSetVariable::updateHardwareDataSet()
	{
		if(mpHardwareDataSet == NULL)
			mpHardwareDataSet = new Verdi::HardwareDataSet();

		mpHardwareDataSet->updateFromDataSet(mpDataSet);
	}

	Verdi::String DataSetVariable::getDataSummary() const
	{
		Verdi::String ret = "";

		ret += "Width: " + Verdi::StringUtility::toString(mpDataSet->getWidth()) + " ";
		ret += "Height: " + Verdi::StringUtility::toString(mpDataSet->getHeight()) + " ";
		ret += "Depth: " + Verdi::StringUtility::toString(mpDataSet->getDepth());

		return ret;
	}

	RenderStrategyVariable::RenderStrategyVariable(Verdi::RenderStrategy* strategy,
		Verdi::String type)
	{
		mpRenderStrategy = strategy;
		mType = type;
	}

	RenderStrategyVariable::~RenderStrategyVariable()
	{
		delete mpRenderStrategy;
	}

	Verdi::String RenderStrategyVariable::getDataSummary() const 
	{
		return mType;
	}
}