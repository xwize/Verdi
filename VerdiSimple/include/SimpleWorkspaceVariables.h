
#ifndef __Simple_Workspace_Variables_H__
#define __Simple_Workspace_Variables_H__

#include "Prerequisites.h"

class DataSetVariable : public Verdi::WorkspaceVariable
{
protected:
	Verdi::DataSet* mpDataSet;
	Verdi::HardwareDataSet* mpHardwareDataSet;
public:

	/** From file constructor.
	*/
	DataSetVariable(Verdi::DataSet* dataSet, const Verdi::String& file = "");

	/** Destructor.
	*/
	~DataSetVariable();

	/** Get the data set.
	*/
	Verdi::DataSet* getDataSet() const { return mpDataSet; }

	/** Update the hardware data set.
	*/
	void updateHardwareDataSet();

	/** Get a string summary of the data.
	*/
	Verdi::String getDataSummary() const;

	/** Get the type of this variable.
	*/
	Verdi::String getType() const { return "DataSet"; }
};

class RenderStrategyVariable : public Verdi::WorkspaceVariable
{
protected:
	Verdi::RenderStrategy* mpRenderStrategy;
	Verdi::String mType;
public:

	/** Constructor.
	*/
	RenderStrategyVariable(Verdi::RenderStrategy* strategy, Verdi::String type);

	/** Destructor.
	*/
	~RenderStrategyVariable();

	/** Get the render strategy.
	*/
	Verdi::RenderStrategy* getRenderStrategy() const { return mpRenderStrategy; }

	/** Get a string summary of the data.
	*/
	Verdi::String getDataSummary() const;

	/** Get the type of this variable.
	*/
	Verdi::String getType() const { return "RenderStrategy"; }
};

#endif // __Simple_Workspace_Variables_H__