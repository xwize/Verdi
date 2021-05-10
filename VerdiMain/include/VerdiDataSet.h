
#ifndef __Verdi_Data_Set_H__
#define __Verdi_Data_Set_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec3.h"

namespace Verdi {

	/** Class for storing a data set in system memory.
	@remarks
		Before uploading any data to the GPU, the system must first
		create or load the data from a file.
		This class provides a mechanism for storing and performing
		computation on this data.
		The data uses an interleaved storage format consisting of channels.
		No semantic information for these channels is provided - for operations 
		that require this information, the user must provide the method with 
		the source data and channel.
	*/
	class DataSet
	{
	public:
		/** The maximum number of channels
			available for a data set. This
			value should be enough for most
			purposes.
		*/
		static const int MAX_NUM_CHANNELS = 3;

		/** Structure for representing 
			sub-windows of the volume data.
		*/
		struct Window
		{
			int x, y, z;
			int width, height, depth;
		};
		/** Enum of available component formats.
		@remarks
			All channels use the same
			component format.
		*/
		enum ChannelFormat {
			DSCF_UBYTE8,
			DSCF_UBYTE16,
			DSCF_UBYTE32,
			DSCF_FLOAT32
		};
	protected:
		uint32_t mWidth;
		uint32_t mHeight;
		uint32_t mDepth;
		uint32_t mNumChannels;
		ChannelFormat mChannelFormat;
		Verdi::Vec3 mScaleMultiplier;
		float mNormalisationFactor;
		void* mpData;
	private:
		/** Private constructor.
		@remarks
			Instances of the class must
			contain actual data.
		*/
		DataSet();
	public:

		/** Constructor for empty data set with a single value
			component.
		*/
		DataSet(uint32_t w, uint32_t h, uint32_t d,
			uint32_t numChannels, ChannelFormat format = DSCF_UBYTE16);

		/** Constructor for existing data.
		@remarks
			Data pointer is not deep copied. Lifetime
			control relinquished from user.
		*/
		DataSet(uint32_t w, uint32_t h, uint32_t d,
			uint32_t numChannels, ChannelFormat format, void* data);

		/** Default destructor.
		*/
		~DataSet();

		/** Copy from a source channel to a destination channel.
		*/
		static void copyChannel(const Window& window, 
			const DataSet* srcData, int srcChannelIndex, 
			DataSet* dstData, int dstChannelIndex);

		/** Compute gradient data.
		*/
		static void computeGradientMagnitudes(const Window& window,
			const DataSet* srcData, int srcChannelIndex, 
			DataSet* dstData, int dstChannelIndex);

		/** Compute min and max values for a window 
			assuming a transfer function that requires
			gradient information has been specified.
		*/
		static void computeMinMaxValues(const Window& window,
			const DataSet* srcData, int srcChannelIndex,
			const DataSet* gradData, int gradChannelIndex,
			const TransferFunction* func,
			float* minOut, float* maxOut);

		/** Compute min and max values for a window 
			based solely on the values within the data set.
		*/
		static void computeMinMaxValues(const Window& window,
			const DataSet* srcData, int srcChannelIndex,
			float* minOut, float* maxOut);

		/** Write a voxel to the given location.
		@remarks
			This method is provided for convenience. The float value is 
			converted to the internal format and written at the specified location. 
			It is not recommended for use where performance
			is critical due to the overhead in converting between formats.
			Using the data pointer and manually filling the values would reduce
			this overhead.
		*/
		void setVoxel(int x, int y, int z, int channelIndex, float val);

		/** Sample the data set at the given location.
		@remarks
			The same can be said for this method as the 
			setVoxel method.
		*/
		float getVoxel(int x, int y, int z, int channelIndex = 0) const;

		/** Set the scaling multiplier.
		*/
		void setScaleMultiplier(const Vec3& v) { mScaleMultiplier = v; }

		/** Get scaling multiplier.
		@remarks
			The scaling multiplier is a vector that describes
			how the voxel grid has been scaled in each
			dimension. This information is typically
			provided from the data set file.
		*/
		Vec3 getScaleMultiplier() const { return mScaleMultiplier; }

		/** Set the value normalisation factor.
		@remarks
			This value is used to divide a voxel value down
			to the 0 - 1 range. 
		*/
		void setNormalisationFactor(float n) { mNormalisationFactor = n; }

		/** Set the value normalisation factor from the channel format.
		*/
		void setNormalisationFactor(ChannelFormat format);

		/** Get the value normalisation factor.
		*/
		float getNormalisationFactor() const { return mNormalisationFactor; }

		/** Get the dimension ratio.
		@remarks
			This is a vector representing the dimensions
			of the voxel grid (independently of scaling factor)
			where the dimensions are scaled so that the largest
			axis is given a value of 1.
		*/
		Vec3 getDimensionRatio() const;

		/** Get corrected dimension ratio.
		@remarks 
			This value can be used to scale
			a unit cube to take the shape of the 
			voxel grid, where the largest axis
			of the voxel grid has a length of
			1 on the cube.
		@returns 
			The adjusted dimension ratio
			using the scaling multiplier.
		*/
		Vec3 getCorrectedDimensionRatio() const;

		/** Get the data pointer.
		@remarks
			Data can be modified and read using this method.
		*/
		void* getData() const { return mpData; }

		/** Get the width.
		*/
		uint32_t getWidth() const { return mWidth; }

		/** Get the height.
		*/
		uint32_t getHeight() const { return mHeight; }

		/** Get the depth.
		*/
		uint32_t getDepth() const { return mDepth; }

		/** Get number of components.
		*/
		uint32_t getNumChannels() const { return mNumChannels; }

		/** Get the format of the components.
		*/
		ChannelFormat getChannelFormat() const { return mChannelFormat; }

		/** Computes the size in bytes of the data.
		*/
		uint32_t getSizeInBytes() const;
	};
}

#endif // __Verdi_Data_Set_H__