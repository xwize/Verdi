
#ifndef __Verdi_Render_Context_H__
#define __Verdi_Render_Context_H__

#include "VerdiPrerequisites.h"
#include "VerdiDataSet.h"
#include "VerdiCamera.h"
#include "VerdiTransferFunction.h"
#include "VerdiHardwareTransferFunction.h"
#include "VerdiCompositor.h"
#include "VerdiHardwareDataSet.h"
#include "VerdiMinMaxOctree.h"
#include "VerdiMaterial.h"
#include "VerdiLight.h"

namespace Verdi {

	/** Class representing a particular configuration of a scene.
		Only the information used during rendering is encapsulated
		in this class.
	*/
	class RenderContext
	{
	public:
		/** Enumeration of possible rendering modes.
			Static modes should be selected when the user
			has uploaded the data set to GPU memory.
			Streaming mode should be selected if the user
			would rather upload the data in "bricks".
		*/
		enum DataMode {
			RCDM_STATIC,
			RCDM_SIMULATION
		};
	protected:
		Camera* mpCamera;
		Compositor mCompositor;
		uint32_t mNumIterations;
		Vec4 mBackgroundColour;
		Vec3 mDimensionRatio;
		Vec3 mDimensions;
		DataSet* mpDataSet;
		DataMode mDataMode;
		HardwareDataSet* mpHardwareDataSet;
		TransferFunction* mpTransferFunction;
		HardwareTransferFunction* mpHardwareTransferFunction;
		MinMaxOctree* mpMinMaxOctree;
		Material mMaterial;
		Light mLight;
		Vec3 mPlaneNormal;
		float mPlaneDistance;
		bool mUseLighting;
		bool mUseERT;
		bool mUsePlaneCut;
	public:

		/** Parameterised constructor.
		*/
		RenderContext(HardwareDataSet* dataSet = NULL, Camera* camera = NULL);

		/** Default destructor.
		*/
		~RenderContext();

		/** Get the material.
		*/
		Light getLight() const { return mLight; }

		/** Set the material.
		*/
		void setLight(const Light& m) { mLight = m; }

		/** Get the material.
		*/
		Material getMaterial() const { return mMaterial; }

		/** Set the material.
		*/
		void setMaterial(const Material& m) { mMaterial = m; }

		/** Set the lighting flag.
		*/
		void setLightingEnabled(bool enabled) { mUseLighting = enabled; }

		/** Get the lighting flag.
		*/
		bool getLightingEnabled() const { return mUseLighting; }

		/** Set the early ray termination.
		*/
		void setEarlyRayTerminationEnabled(bool enabled) { mUseERT = enabled; }

		/** Get the early ray termination flag.
		*/
		bool getEarlyRayTerminationEnabled() const { return mUseERT; }

		/** Set the compositor.
		*/
		void setCompositor(const Compositor& compositor) { mCompositor = compositor; }

		/** Get the compositor.
		*/
		Compositor getCompositor() const { return mCompositor; }

		/** Get the draw mode.
		*/
		DataMode getDataMode() const { return mDataMode; }

		/** Set the empty space tree.
		*/
		void setMinMaxOctree(MinMaxOctree* tree) { mpMinMaxOctree = tree; }

		/** Get the empty space tree.
		*/
		MinMaxOctree* getMinMaxOctree() const { return mpMinMaxOctree; }

		/** Set the transfer function.
		*/
		void setTransferFunction(TransferFunction* func) {
			mpTransferFunction = func; }

		/** Get the transfer function.
		*/
		TransferFunction* getTransferFunction() const { 
			return mpTransferFunction; }

		/** Set the hardware transfer function.
		*/
		void setHardwareTransferFunction(HardwareTransferFunction* func) {
			mpHardwareTransferFunction = func; }

		/** Get the hardware transfer function.
		*/
		HardwareTransferFunction* getHardwareTransferFunction() const { 
			return mpHardwareTransferFunction; }

		/** Set the dimension ratio.
		*/
		void setDimensionRatio(const Vec3& ratio) { mDimensionRatio = ratio; }

		/** Get the dimension ratio.
		*/
		Vec3 getDimensionRatio() const { return mDimensionRatio; }

		/** Set the dimensions.
		*/
		void setDimensions(const Vec3& dim) { mDimensions = dim; }

		/** Get the dimension ratio.
		*/
		Vec3 getDimensions() const { return mDimensions; }

		/** Set the data set.
		*/
		void setDataSet(DataSet* dataSet) { mpDataSet = dataSet; }

		/** Get the data set.
		*/
		DataSet* getDataSet() const { return mpDataSet; }

		/** Set the hardware data set.
		*/
		void setHardwareDataSet(HardwareDataSet* dataSet) { mpHardwareDataSet = dataSet; }

		/** Get the hardware data set.
		*/
		HardwareDataSet* getHardwareDataSet() const { return mpHardwareDataSet; }

		/** Set the background colour.
		*/
		void setBackgroundColour(Vec4 colour) { mBackgroundColour = colour; }

		/** Set the number of iterations.
		*/
		void setNumIterations(uint32_t i) { mNumIterations = i; }

		/** Get the background colour.
		*/
		Vec4 getBackgroundColour() const { return mBackgroundColour; }

		/** Get the number of selected iterations.
		*/
		uint32_t getNumIterations() const { return mNumIterations; }

		/** Get the camera.
		*/
		Camera* getCamera() const { return mpCamera; }

		/** Attach the camera.
		*/
		void setCamera(Camera* camera) { mpCamera = camera; }

		/** Checks to see if the current state of the context
			is a valid context.
		*/
		bool isValid() const;
	};
}

#endif // __Verdi_Render_Context_H__