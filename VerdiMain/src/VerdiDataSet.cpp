
#include "VerdiDataSet.h"
#include "VerdiException.h"
#include "VerdiTransferFunction.h"

namespace Verdi {

	DataSet::DataSet()
	{

	}

	DataSet::DataSet(uint32_t w, uint32_t h, uint32_t d,
		 uint32_t numChannels, ChannelFormat format) :
	mWidth(w), mHeight(h), mDepth(d), mNumChannels(numChannels), mChannelFormat(format),
		mScaleMultiplier(1.0f,1.0f,1.0f)
	{
		if(mNumChannels < 1 || mNumChannels > 3)
			VERDI_THROW(Verdi::ParameterInvalidException,"Number of components must be between 1 and 3",1);

		int numElements = mWidth*mHeight*mDepth*mNumChannels;

		switch(format)
		{
		case DSCF_UBYTE8 : mpData	= new uint8_t[numElements*1]; break;
		case DSCF_UBYTE16 : mpData	= new uint8_t[numElements*2]; break;
		case DSCF_UBYTE32 : mpData	= new uint8_t[numElements*4]; break;
		case DSCF_FLOAT32 : mpData	= new float[numElements]; break;
		}

		setNormalisationFactor(format);
	}

	DataSet::DataSet(uint32_t w, uint32_t h, uint32_t d,
		uint32_t numChannels, ChannelFormat format, void* data) :
	mWidth(w), mHeight(h), mDepth(d), mNumChannels(numChannels), mChannelFormat(format), mpData(data),
		mScaleMultiplier(1.0f,1.0f,1.0f)
	{
		if(mpData == NULL)
			VERDI_THROW(Verdi::ParameterInvalidException,"Data cannot be null",0);

		if(mNumChannels < 1 || mNumChannels > MAX_NUM_CHANNELS)
			VERDI_THROW(Verdi::ParameterInvalidException,"Invalid number of channels",1);

		setNormalisationFactor(format);
	}

	DataSet::~DataSet()
	{
		VERDI_LOG("Destructor called.");

		if(mpData != NULL)
		{
			switch(mChannelFormat)
			{
				case DSCF_UBYTE8  : delete [] (uint8_t*) mpData; break;
				case DSCF_UBYTE16 : delete [] (uint8_t*)mpData; break;
				case DSCF_UBYTE32 : delete [] (uint8_t*)mpData; break;
				case DSCF_FLOAT32 : delete [] (float*)mpData; break;
			}
		}
	}

	void DataSet::setNormalisationFactor(ChannelFormat format)
	{
		switch(format)
		{
		case DSCF_UBYTE8 : mNormalisationFactor = powf(2.0f,8.0f) - 1.0f; break;
		case DSCF_UBYTE16 : mNormalisationFactor = powf(2.0f,16.0f) - 1.0f; break;
		case DSCF_UBYTE32 : mNormalisationFactor = powf(2.0f,32.0f) - 1.0f; break;
		case DSCF_FLOAT32 : mNormalisationFactor = 1.0f; break;
		}
	}

	uint32_t DataSet::getSizeInBytes() const
	{
		int numElements = mWidth*mHeight*mDepth*mNumChannels;

		switch(mChannelFormat)
		{
		case DSCF_UBYTE8 : return numElements; break;
		case DSCF_UBYTE16 : return numElements*2; break;
		case DSCF_UBYTE32 : return numElements*4; break;
		case DSCF_FLOAT32 : return numElements*4; break;
		}

		return 0;
	}

	Vec3 DataSet::getDimensionRatio() const
	{
		float fw = (float)mWidth;
		float fh = (float)mHeight;
		float fd = (float)mDepth;

		float fm = fw > fh ? fw : fh;
		fm = fm > fd ? fm : fd;

		return Vec3(fw/fm,fh/fm,fd/fm);
	}

	Vec3 DataSet::getCorrectedDimensionRatio() const
	{
		float fw = (float)mWidth * mScaleMultiplier.x;
		float fh = (float)mHeight * mScaleMultiplier.y;
		float fd = (float)mDepth * mScaleMultiplier.z;

		float fm = fw > fh ? fw : fh;
		fm = fm > fd ? fm : fd;

		return Vec3(fw/fm,fh/fm,fd/fm);
	}

	void DataSet::copyChannel(const Window& window, const DataSet* srcData, int srcChannelIndex,
			DataSet* dstData, int dstChannelIndex)
	{
		for(int k = window.z; k < window.z + window.depth; ++k)
		{
			for(int j = window.y; j < window.y + window.height; ++j)
			{
				for(int i = window.x; i < window.x + window.width; ++i)
				{
					dstData->setVoxel(i,j,k,dstChannelIndex,srcData->getVoxel(i,j,k,srcChannelIndex));
				}
			}
		}
	}

	void DataSet::setVoxel(int x, int y, int z, int channelIndex, float val)
	{
		val *= mNormalisationFactor;

		int loc = x + y * mWidth + z * mWidth * mHeight + channelIndex;

		if(mChannelFormat == DSCF_FLOAT32)
			((float*)mpData)[loc] = val;

		else if(mChannelFormat == DSCF_UBYTE8)
			((uint8_t*)mpData)[loc] = (uint8_t)val;

		else if(mChannelFormat == DSCF_UBYTE16)
			((uint16_t*)mpData)[loc] = (uint16_t)val;

		else if(mChannelFormat == DSCF_UBYTE32)
			((uint32_t*)mpData)[loc] = (uint32_t)val;
	}

	float DataSet::getVoxel(int x, int y, int z, int channelIndex) const
	{
		float val = 0.0f;

		int loc = x + y * mWidth + z * mWidth * mHeight + channelIndex;

		if(mChannelFormat == DSCF_FLOAT32)
			val = ((float*)mpData)[loc];

		else if(mChannelFormat == DSCF_UBYTE8)
			val = (float)((uint8_t*)mpData)[loc];

		else if(mChannelFormat == DSCF_UBYTE16)
			val = (float)((uint16_t*)mpData)[loc];

		else if(mChannelFormat == DSCF_UBYTE32)
			val = (float)((uint32_t*)mpData)[loc];

		return val / mNormalisationFactor;
	}

	void DataSet::computeGradientMagnitudes(const Window& window, const DataSet* srcData, int srcChannelIndex, 
		DataSet* dstData, int dstChannelIndex)
	{
		#pragma omp parallel for
		for(int k = window.z; k < window.z + window.depth; ++k)
		{
			for(int j = window.y; j < window.y + window.height; ++j)
			{
				for(int i = window.x; i < window.x + window.width; ++i)
				{
					int pi = Math::maximum<int>(i-1,0);
					int ni = Math::minimum<int>(i+1,window.width-1);

					int pj = Math::maximum<int>(j-1,0);
					int nj = Math::minimum<int>(j+1,window.height-1);

					int pk = Math::maximum<int>(k-1,0);
					int nk = Math::minimum<int>(k+1,window.depth-1);

					float xl = srcData->getVoxel(pi,j,k,srcChannelIndex);  
					float xr = srcData->getVoxel(ni,j,k,srcChannelIndex); 

					float yl = srcData->getVoxel(i,pj,k,srcChannelIndex);  
					float yr = srcData->getVoxel(i,nj,k,srcChannelIndex); 

					float zl = srcData->getVoxel(i,j,pk,srcChannelIndex);  
					float zr = srcData->getVoxel(i,j,nk,srcChannelIndex); 

					Vec3 grad;
					grad.x = xl - xr;
					grad.y = yl - yr;
					grad.z = zl - zr;

					float len = grad.length();

					dstData->setVoxel(i,j,k,dstChannelIndex, len);
				}
			}
		}
	}

	void DataSet::computeMinMaxValues(const Window& window, const DataSet* srcData,
		int srcChannelIndex, const DataSet* gradData, int gradChannelIndex, 
		const TransferFunction* func, float* minOut, float* maxOut)
	{
		float minV = Math::INFINITY_POSITIVE;
		float maxV = Math::INFINITY_NEGATIVE;

		for(int k = window.z; k < window.z + window.depth; ++k)
		{
			for(int j = window.y; j < window.y + window.height; ++j)
			{
				for(int i = window.x; i < window.x + window.width; ++i)
				{
					float v = srcData->getVoxel(i,j,k,srcChannelIndex);
					float g = gradData->getVoxel(i,j,k,gradChannelIndex);

					float val = func->sample(v,g).w;

					minV = Math::minimum<float>(val,minV);
					maxV = Math::maximum<float>(val,maxV);
				}
			}
		}

		(*minOut) = minV;
		(*maxOut) = maxV;
	}

	void DataSet::computeMinMaxValues(const Window& window, const DataSet* srcData,
		int srcChannelIndex, float* minOut, float* maxOut)
	{
		float minV = Math::INFINITY_POSITIVE;
		float maxV = Math::INFINITY_NEGATIVE;

		for(int k = window.z; k < window.z + window.depth; ++k)
		{
			for(int j = window.y; j < window.y + window.height; ++j)
			{
				for(int i = window.x; i < window.x + window.width; ++i)
				{
					float v = srcData->getVoxel(i,j,k,srcChannelIndex);
					float val = v;

					minV = Math::minimum<float>(val,minV);
					maxV = Math::maximum<float>(val,maxV);
				}
			}
		}

		(*minOut) = minV;
		(*maxOut) = maxV;
	}

	/*
	/*DataSet* DataSet::computeGradients() const
	{
		assert(mNumChannels == 1);
		assert(mChannelFormat == DSCF_UBYTE8);

		// Algorithm outline.
		// The Sobel operator can be represented
		// by a linearly seperable filter kernel.
		// For a fast-ish implementation this means
		// doing the x y and z convolutions in 1d
		// in separate passes which operate on the 
		// intermediate results of the previous pass.

		// finite differences in x, "smooth" y, "smooth" z, <- x component
		// finite differences in y, "smooth" x, "smooth" z, <- y component
		// finite differences in z, "smooth" x, "smooth" y, <- z component

		unsigned char* pchData = (unsigned char*)mpData;

		Vec3* gradients = new Vec3[mWidth*mHeight*mDepth];
		Vec3* swapGradients = new Vec3[mWidth*mHeight*mDepth];

		float scaleFactor = 1.0f/(powf(2.0f,8.0f)-1.0f);

		// First pass compute finite differences
		// Hooray! 
		for(int i = 0; i < (int)mWidth; i++)
		{
			for(int j = 0; j < (int)mHeight; j++)
			{
				for(int k = 0; k < (int)mDepth; k++)
				{
					int pi = max(i-1,0);
					int ni = min(i+1,mWidth-1);

					int pj = max(j-1,0);
					int nj = min(j+1,mHeight-1);

					int pk = max(k-1,0);
					int nk = min(k+1,mDepth-1);

					float xl = pchData[pi+(j)*mWidth + (k)*mHeight*mWidth];
					float xr = pchData[ni+(j)*mWidth + (k)*mHeight*mWidth]; 

					float yl = pchData[(i)+ pj*mWidth + (k)*mHeight*mWidth];
					float yr = pchData[(i)+ nj*mWidth + (k)*mHeight*mWidth]; 

					float zl = pchData[(i)+(j)*mWidth + pk*mHeight*mWidth];
					float zr = pchData[(i)+(j)*mWidth + nk*mHeight*mWidth]; 

					float x = 0.5f*(xl - xr)*scaleFactor;
					float y = 0.5f*(yl - yr)*scaleFactor; 
					float z = 0.5f*(zl - zr)*scaleFactor; 

					gradients[i+j*mWidth + k*mHeight*mWidth].x = x;
					gradients[i+j*mWidth + k*mHeight*mWidth].y = y;
					gradients[i+j*mWidth + k*mHeight*mWidth].z = z;
					
					swapGradients[i+j*mWidth + k*mHeight*mWidth].x = x;
					swapGradients[i+j*mWidth + k*mHeight*mWidth].y = y;
					swapGradients[i+j*mWidth + k*mHeight*mWidth].z = z;
				}
			}
		}

		// Smooth x components in y
		// Smooth y components in x
		// Smooth z components in x
		for(unsigned int i = 1; i < mWidth-1; ++i)
		{
			for(unsigned int j = 1; j < mHeight-1; j++)
			{
				for(unsigned int k = 1; k < mDepth-1; k++)
				{
					float x1 = gradients[(i)+(j-1)*mWidth + (k)*mHeight*mWidth].x;
					float x2 = 2.0f * gradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].x;
					float x3 = gradients[(i)+(j+1)*mWidth + (k)*mHeight*mWidth].x;
					float xs = (x1 + x2 + x3)*0.25f;

					float y1 = gradients[(i-1)+(j)*mWidth + (k)*mHeight*mWidth].y;
					float y2 = 2.0f * gradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].y;
					float y3 = gradients[(i+1)+(j)*mWidth + (k)*mHeight*mWidth].y;
					float ys = (y1 + y2 + y3)*0.25f;

					float z1 = gradients[(i-1)+(j)*mWidth + (k)*mHeight*mWidth].z;
					float z2 = 2.0f * gradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].z;
					float z3 = gradients[(i+1)+(j)*mWidth + (k)*mHeight*mWidth].z;
					float zs = (z1 + z2 + z3)*0.25f;

					swapGradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].x = xs;
					swapGradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].y = ys;
					swapGradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].z = zs;

				}
			}
		}

		// Smooth x components in z
		// Smooth y components in z
		// Smooth z components in y
		for(unsigned int i = 1; i < mWidth-1; ++i)
		{
			for(unsigned int j = 1; j < mHeight-1; j++)
			{
				for(unsigned int k = 1; k < mDepth-1; k++)
				{
					float x1 = swapGradients[(i)+(j)*mWidth + (k-1)*mHeight*mWidth].x;
					float x2 = 2.0f * swapGradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].x;
					float x3 = swapGradients[(i)+(j)*mWidth + (k+1)*mHeight*mWidth].x;
					float xs = (x1 + x2 + x3)*0.25f;

					float y1 = swapGradients[(i)+(j)*mWidth + (k-1)*mHeight*mWidth].y;
					float y2 = 2.0f * swapGradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].y;
					float y3 = swapGradients[(i)+(j)*mWidth + (k+1)*mHeight*mWidth].y;
					float ys = (y1 + y2 + y3)*0.25f;

					float z1 = swapGradients[(i)+(j-1)*mWidth + (k)*mHeight*mWidth].z;
					float z2 = 2.0f * swapGradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].z;
					float z3 = swapGradients[(i)+(j+1)*mWidth + (k)*mHeight*mWidth].z;
					float zs = (z1 + z2 + z3)*0.25f;

					gradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].x = xs;
					gradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].y = ys;
					gradients[(i)+(j)*mWidth + (k)*mHeight*mWidth].z = zs;

				}
			}
		}

		delete [] swapGradients;

		float maxGradMag = 0.0f;

		// Encoding step
		for(unsigned int i = 0; i < mWidth*mHeight*mDepth; ++i)
		{
			maxGradMag = max( maxGradMag, gradients[i].length() );
			gradients[i] += 1.0f;
			gradients[i] *= (0.5f);
		}

		printf("max: %f\n",maxGradMag);
		return new DataSet(mWidth,mHeight,mDepth,3,DSCF_FLOAT32,gradients);
	}*/
}