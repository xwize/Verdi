
#include "VerdiTransferFunction.h"

namespace Verdi {

	TransferFunction::TransferFunction(Vec4 initialValue, uint32_t width, uint32_t height)
		: mWidth(width), mHeight(height)
	{
		mpData = new ChannelType[mWidth*mHeight*4];
		clear(initialValue);
	}

	TransferFunction::~TransferFunction()
	{
		VERDI_LOG("Destructor called.");
		delete [] mpData;
		mpData = NULL;
	}

	Vec4 TransferFunction::getPixel(int i, int j) const
	{
		Vec4 ret;
		ret.x = mpData[(i+j*mWidth)*4+0];
		ret.y = mpData[(i+j*mWidth)*4+1];
		ret.z = mpData[(i+j*mWidth)*4+2];
		ret.w = mpData[(i+j*mWidth)*4+3];

		ret *= (1.0f/(float)(CHANNEL_MAX_VALUE));
		return ret;
	}

	void TransferFunction::setPixel(int i, int j, const Vec4& v)
	{
		Vec4 wrt = v;
		wrt *= (float)(CHANNEL_MAX_VALUE);

		mpData[(i+j*mWidth)*4+0] = (ChannelType)wrt.values[0];
		mpData[(i+j*mWidth)*4+1] = (ChannelType)wrt.values[1];
		mpData[(i+j*mWidth)*4+2] = (ChannelType)wrt.values[2];
		mpData[(i+j*mWidth)*4+3] = (ChannelType)wrt.values[3];
	}

	void TransferFunction::clear(Vec4 value) 
	{
		for(uint32_t j = 0; j < mHeight; ++j) {
			for(uint32_t i = 0; i < mWidth; ++i) {
				setPixel(i,j,value);
			}
		}
	}

	uint32_t TransferFunction::getSizeInBytes() const 
	{
		return mWidth*mHeight*sizeof(ChannelType)*4;
	}

	bool TransferFunction::clip(const Window& winIn, Window& winOut) const
	{
		int x0 = winIn.x;
		int y0 = winIn.y;
		int x1 = winIn.x + winIn.width - 1;
		int y1 = winIn.y + winIn.height - 1;

		if(winIn.width <= 0 || winIn.height <= 0)
			return false;

		if(x0 >= (int)mWidth || y0 >= (int)mHeight)
			return false;

		if(x1 < 0 || y1 < 0)
			return false;

		x0 = Verdi::Math::clamp<int>(x0,0,mWidth-1);
		y0 = Verdi::Math::clamp<int>(y0,0,mHeight-1);

		x1 = Verdi::Math::clamp<int>(x1,0,mWidth-1);
		y1 = Verdi::Math::clamp<int>(y1,0,mHeight-1);

		winOut.x = x0;
		winOut.y = y0;

		winOut.width = 1 + x1 - x0;
		winOut.height = 1 + y1 - y0;

		return true;
	}

	void TransferFunction::gaussianSplat(const Window& win, int channelMask,
		float intensity, float peak, WriteMode wm)
	{
		float wsrc = 1.0f;
		float wdst = 1.0f;

		switch(wm) {
		case TFWM_SET : wsrc = 0.0f; wdst = 1.0f; break;
		case TFWM_SUB : wsrc = 1.0f; wdst = -1.0f; break;
		case TFWM_ADD : wsrc = 1.0f; wdst = 1.0f; break;
		default : break;
		}

		// Clip the window in here
		Window clippedWin;
		if(!clip(win,clippedWin))
			return;

		float dfx = 2.0f/(float)(win.width - 1);
		float dfy = 2.0f/(float)(win.height - 1);

		float sfx = -1.0f + dfx * (clippedWin.x - win.x);
		float sfy = -1.0f + dfy * (clippedWin.y - win.y);

		if(win.width == 1) {
			dfx = 0.0f;
			sfx = 0.0f;
		}

		if(win.height == 1) {
			dfy = 0.0f;
			sfy = 0.0f;
		}

		float fx = sfx;
		float fy = sfy;

		for(int j = clippedWin.y; j < clippedWin.y + clippedWin.height; ++j) {

			for(int i = clippedWin.x; i < clippedWin.x + clippedWin.width; ++i) {

				fx = sfx + static_cast<float>(i - clippedWin.x) * dfx;
				fy = sfy + static_cast<float>(j - clippedWin.y) * dfy;

				fx = Verdi::Math::clamp<float>(fx,-1.0f,1.0f);
				fy = Verdi::Math::clamp<float>(fy,-1.0f,1.0f);

				// Compute two dimensional Gaussian
				float vdst = intensity * expf(-( ((fx*fx) + (fy*fy))/(2.0f*peak*peak) ) );

				// We need to multiply by a function that has 0 around the edges of the window.
				vdst = Verdi::Math::clamp(vdst*sqrtf((1-(fx*fx))*(1-(fy*fy))),0.0f,1.0f);

				for(int k = 0; k < 4; ++k) {

					if(!(channelMask & (1 << k)))
						continue;

					float vsrc = ((float)mpData[(i + j*mWidth)*4 + k])/(float)(CHANNEL_MAX_VALUE);

					float vcomp = Verdi::Math::clamp<float>(
						vsrc*wsrc + vdst*wdst, 0.0f,1.0f);

					int wval = Verdi::Math::clamp<int>(int(vcomp*(float)CHANNEL_MAX_VALUE),
						0,(int)CHANNEL_MAX_VALUE );

					mpData[(i+j*mWidth)*4 + k] = (ChannelType)wval;
				}
			}
		}
	}

	Vec4 TransferFunction::sample(float x, float y) const
	{
		// Assume x and y in range 0 -> 1 incl.
		float sx = x * (float)(mWidth-1);
		float sy = y * (float)(mHeight-1);

		float fracsx = sx - floorf(sx);
		float fracsy = sy - floorf(sy);

		int ax = (int)floorf(sx);
		int bx = (int)ceilf(sx);
		int cx = ax;
		int dx = bx;

		int ay = (int)floorf(sy);
		int by = ay;
		int cy = (int)ceilf(sy);
		int dy = cy;

		Vec4 a,b,c,d;

		a = getPixel(ax,ay);
		b = getPixel(bx,by);
		c = getPixel(cx,cy);
		d = getPixel(dx,dy);

		Vec4 ab = Math::linearInterpolate(a,b,fracsx);
		Vec4 cd = Math::linearInterpolate(c,d,fracsx);

		return Math::linearInterpolate(ab,cd,fracsy);
	}

	void TransferFunction::fillFromSpline(const Spline& spline, Axis selectedAxis)
	{
		// There is inefficiency - the spline gets
		// sampled at the same positions multiple times
		// probably better to just sample once and copy
		// the results multiple times, but seeing as
		// this code should be pretty fast anyway
		// it's not a huge deal.
		if(selectedAxis == TFA_X_AXIS)
		{
			for(uint32_t i = 0; i < mHeight; ++i)
			{
				for(uint32_t j = 0; j < mWidth; ++j)
				{
					float fj = (float)j/(float)(mWidth-1);
					//mpData[i*mWidth + j] = spline.sample(fj);
					setPixel(j,i, spline.sample(fj) );
				}
			}
		}
		else
		{
			for(uint32_t i = 0; i < mHeight; ++i)
			{
				for(uint32_t j = 0; j < mWidth; ++j)
				{
					float fi = (float)i/(float)(mHeight-1);
					//mpData[i*mWidth + j] = spline.sample(fi);
					setPixel(j,i, spline.sample(fi) );
				}
			}
		}
	}
}