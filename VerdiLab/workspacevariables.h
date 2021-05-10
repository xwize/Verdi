#ifndef WORKSPACEVARIABLES_H
#define WORKSPACEVARIABLES_H

#include "VerdiWorkspaceVariable.h"
#include "Verdi.h"

class SimulationVariable : public Verdi::WorkspaceVariable
{
public:
    typedef void(*SimCreateFunc)(uint32_t* dimXOut,
                                 uint32_t* dimYOut,
                                 uint32_t* dimZOut,
                                 uint32_t* texOut);
    typedef void(*SimUpdateFunc)();
    typedef void(*SimDestroyFunc)();
protected:
    // Simulation
    SimCreateFunc* mpCreateFunc;
    SimUpdateFunc* mpUpdateFunc;
    SimDestroyFunc* mpDestroyFunc;
    uint32_t mWidth, mHeight, mDepth;
    Verdi::HardwareDataSet* mpHardwareDataSet;
public:

    /** From sim constructor.
     */
    SimulationVariable(SimCreateFunc* pCreate,
                    SimUpdateFunc* pUpdate,
                    SimDestroyFunc* pDestroy,
                    const Verdi::String& file = "");

    ~SimulationVariable();

    uint32_t getWidth() const   { return mWidth; }
    uint32_t getHeight() const  { return mHeight; }
    uint32_t getDepth() const   { return mDepth; }

    /** Get the hardware data set.
    */
    Verdi::HardwareDataSet* getHardwareDataSet() const {
        return mpHardwareDataSet;
    }

    /** Get a string summary of the data.
    */
    Verdi::String getDataSummary() const;

    /** Get the type of this variable.
    */
    Verdi::String getType() const { return "Simulation"; }
};

class DataSetVariable : public Verdi::WorkspaceVariable
{
public:

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

    /** Get the hardware data set.
    */
    Verdi::HardwareDataSet* getHardwareDataSet() const {
        return mpHardwareDataSet;
    }

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

class TransferFunctionVariable : public Verdi::WorkspaceVariable
{
protected:
    Verdi::TransferFunction* mpTransferFunction;
    Verdi::HardwareTransferFunction* mpHardwareTransferFunction;
    bool mIsDirty;
public:

    /** From file constructor.
    */
    TransferFunctionVariable(Verdi::TransferFunction* TransferFunction, const Verdi::String& file = "");

    /** Destructor.
    */
    ~TransferFunctionVariable();

    /** Get the transfer function.
    */
    Verdi::TransferFunction* getTransferFunction() const { return mpTransferFunction; }

    /** Get the transfer function.
    */
    Verdi::HardwareTransferFunction* getHardwareTransferFunction() const {
        return mpHardwareTransferFunction;
    }

    /** Update the hardware transfer function set.
    */
    void updateHardwareTransferFunction();

    /** Mark for update.
    */
    void markForUpdate() { mIsDirty = true; }

    /** Returns true if the hardware transfer function needs an
        update.
    */
    bool needsUpdate() const { return mIsDirty; }

    /** Get a string summary of the data.
    */
    Verdi::String getDataSummary() const;

    /** Get the type of this variable.
    */
    Verdi::String getType() const { return "TransferFunction"; }
};

class MinMaxOctreeVariable : public Verdi::WorkspaceVariable
{
protected:
    Verdi::MinMaxOctree* mpOctree;
public:

    /** Constructor.
    */
    MinMaxOctreeVariable(Verdi::MinMaxOctree* minMaxOctree);

    /** Destructor.
    */
    ~MinMaxOctreeVariable();

    /** Get the minmax octree.
     */
    Verdi::MinMaxOctree* getMinMaxOctree() const { return mpOctree; }

    /** Get a string summary of the data.
    */
    Verdi::String getDataSummary() const;

    /** Get the type of this variable.
    */
    Verdi::String getType() const { return "MinMaxOctree"; }
};

#endif // WORKSPACEVARIABLES_H
