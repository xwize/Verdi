
#include "workspacevariables.h"

SimulationVariable::SimulationVariable(SimCreateFunc *pCreate,
                                 SimUpdateFunc *pUpdate,
                                 SimDestroyFunc *pDestroy,
                                 const Verdi::String& file)
{
    mpHardwareDataSet = NULL;
    mFileName = file;

    mpCreateFunc = pCreate;
    mpUpdateFunc = pUpdate;
    mpDestroyFunc = pDestroy;

    if(!mFileName.empty())
        mIsFromFile = true;

    assert(mpCreateFunc != NULL);
    assert(mpDestroyFunc != NULL);

    uint32_t uiTex = 0;

    (*mpCreateFunc)(&mWidth,&mHeight,&mDepth,&uiTex);

    mpHardwareDataSet = new Verdi::HardwareDataSet(uiTex);
}

SimulationVariable::~SimulationVariable()
{
    (*mpDestroyFunc)();
    delete mpHardwareDataSet;
}

Verdi::String SimulationVariable::getDataSummary() const
{
    Verdi::String ret = "";

    /*ret += "Width: " + Verdi::StringUtility::toString(mpDataSet->getWidth()) + " ";
    ret += "Height: " + Verdi::StringUtility::toString(mpDataSet->getHeight()) + " ";
    ret += "Depth: " + Verdi::StringUtility::toString(mpDataSet->getDepth());*/

    return ret;
}

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

    if(mpDataSet != NULL && mpDataSet->getData() != NULL)
    {
        mpHardwareDataSet->clear();
        mpHardwareDataSet->updateFromDataSet(mpDataSet);
    }
}

Verdi::String DataSetVariable::getDataSummary() const
{
    Verdi::String ret = "";

    ret += "Width: " + Verdi::StringUtility::toString(mpDataSet->getWidth()) + " ";
    ret += "Height: " + Verdi::StringUtility::toString(mpDataSet->getHeight()) + " ";
    ret += "Depth: " + Verdi::StringUtility::toString(mpDataSet->getDepth());

    return ret;
}

TransferFunctionVariable::TransferFunctionVariable(Verdi::TransferFunction* TransferFunction, const Verdi::String& file)
{
    mpTransferFunction = TransferFunction;
    mpHardwareTransferFunction = NULL;
    mFileName = file;
    mIsDirty = true;

    if(!mFileName.empty())
        mIsFromFile = true;
}

TransferFunctionVariable::~TransferFunctionVariable()
{
    delete mpHardwareTransferFunction;
    delete mpTransferFunction;
}

void TransferFunctionVariable::updateHardwareTransferFunction()
{
    if(mpHardwareTransferFunction == NULL)
        mpHardwareTransferFunction = new Verdi::HardwareTransferFunction();

    mpHardwareTransferFunction->updateFromTransferFunction(mpTransferFunction);
    mIsDirty = false;
}

Verdi::String TransferFunctionVariable::getDataSummary() const
{
    Verdi::String ret = "";

    ret += "Width: " + Verdi::StringUtility::toString(mpTransferFunction->getWidth()) + " ";
    ret += "Height: " + Verdi::StringUtility::toString(mpTransferFunction->getHeight()) + " ";

    return ret;
}

MinMaxOctreeVariable::MinMaxOctreeVariable(Verdi::MinMaxOctree *minMaxOctree)
{
    mpOctree = minMaxOctree;
}

MinMaxOctreeVariable::~MinMaxOctreeVariable()
{
    delete mpOctree;
}

Verdi::String MinMaxOctreeVariable::getDataSummary() const
{
    return "";
}
