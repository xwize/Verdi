
#ifndef __Verdi_Transfer_Function_H__
#define __Verdi_Transfer_Function_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec4.h"
#include "VerdiSpline.h"

namespace Verdi {

	/** Class for storing 2d transfer functions in system memory.
		Initially this class supported only 1d transfer functions,
		however the problem with 1d transfer functions is that
		they make it difficult to isolate areas of the data set.
		This class now supports 2d transfer functions, where
		opacity and the volume gradient magnitude are represented
		on the x and y axes respectively.
	*/
	class TransferFunction
	{
	public:
		/** Typedef for the channel elements.
		*/
		typedef uint16_t ChannelType;

		// Max value for the channel type
		static const int CHANNEL_MAX_VALUE = (0x01 << 0x08*sizeof(ChannelType)) - 1;

		/** Structure for representing 
			sub-windows of the data.
		*/
		struct Window
		{
			int x, y;
			int width, height;
		};
	public:
		/** Enumeration of the axes 
			used in the transfer function.
		*/
		enum Axis {
			TFA_X_AXIS	= 1,
			TFA_Y_AXIS	= 2
		};
		/** Enumeration of write modes
			used for rendering to the transfer
			function.
		*/
		enum WriteMode {
			TFWM_SET,
			TFWM_ADD,
			TFWM_SUB
		};
	protected:
		ChannelType* mpData;
		uint32_t mWidth;
		uint32_t mHeight;
	public:

		/** Parameterised constructor.
		*/
		TransferFunction(Vec4 initialValue = Vec4(0.0f,0.0f,0.0f,1.0f),
			uint32_t width = 256, uint32_t height = 32);

		/** Default destructor.
		*/
		~TransferFunction();

		/** Clear the values to the specified vector.
		*/
		void clear(Vec4 value);

		/** Clip an arbitrary window against the bounds
			of this transfer function.
		@returns true if the clipped window is within 
			the bounds
		*/
		bool clip(const Window& winIn, Window& winOut) const;

		/** Sample the transfer function with bilinear interpolation
			at the specified coordinates.
		*/
		Vec4 sample(float x, float y) const;

		/** Set a pixel at the specified location.
		*/
		void setPixel(int i, int j, const Vec4& v);

		/** Get a pixel at the specified location.
		*/
		Vec4 getPixel(int i, int j) const;

		/** Samples a spline for colour values.
			An axis is chosen, and the 1d spline is sampled along that axis
			and duplicated along the texture.
		*/
		void fillFromSpline(const Spline& spline, Axis selectedAxis = TFA_X_AXIS);

		/** Writes a Gaussian splat to the chosen channels.
			Each channel is encoded by a set bit.
		@param intensity Chosen intensity value.
		*/
		void gaussianSplat(const Window& win, int channelMask,
			float intensity, float peak = 0.707f, WriteMode wm = TFWM_SET);

		/** Get the current resolution width.
		*/
		uint32_t getWidth() const { return mWidth; }

		/** Get the current resolution height.
		*/
		uint32_t getHeight() const { return mHeight; }

		/** Get the data.
		*/
		ChannelType* getData() const { return mpData; }

		/** Get the size in bytes.
		*/
		uint32_t getSizeInBytes() const;
	};
}

#endif // __Verdi_Transfer_Function_H___