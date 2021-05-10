
#include "VerdiHardwareDataSet.h"
#include "VerdiException.h"

// We also need GL, GLU and GLee.
#include <GLee.h>

namespace Verdi {

	HardwareDataSet::~HardwareDataSet()
	{
		VERDI_LOG("Destructor called.");
		clear();
	}

	void HardwareDataSet::clear()
	{
		if(mHardwareId != 0)
		{
			glDeleteTextures(1,&mHardwareId);
			mHardwareId = 0;
		}
	}

	void HardwareDataSet::updateFromDataSet(const DataSet* dataSet)
	{
		if(dataSet == NULL)
			VERDI_THROW(Verdi::ParameterInvalidException,"Parameter dataSet must not be NULL.",0);

		if(mHardwareId == 0)
			glGenTextures(1,&mHardwareId);
		
		glBindTexture(GL_TEXTURE_3D,mHardwareId);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		int32_t sourceType = 0;
		int32_t format = 0;
		int32_t internalFormat = 0;

		DataSet::ChannelFormat fmt = dataSet->getChannelFormat();
		switch(fmt)
		{
		case DataSet::DSCF_UBYTE8 : sourceType = GL_UNSIGNED_BYTE; break;
		case DataSet::DSCF_UBYTE16 : sourceType = GL_UNSIGNED_SHORT; break;
		case DataSet::DSCF_UBYTE32 : sourceType = GL_UNSIGNED_INT; break;
		case DataSet::DSCF_FLOAT32 : sourceType = GL_FLOAT; break;
		}

		if(dataSet->getNumChannels() == 1)
		{
			format = GL_RED;
			internalFormat = (fmt == DataSet::DSCF_UBYTE8) ? GL_R8 : GL_R16;
		}

		if(dataSet->getNumChannels() == 2)
		{
			format = GL_RG;
			internalFormat = (fmt == DataSet::DSCF_UBYTE8) ? GL_RG8 : GL_RG16;
		}

		if(dataSet->getNumChannels() == 3)
		{
			format = GL_RGB;
			internalFormat = (fmt == DataSet::DSCF_UBYTE8) ? GL_RGB8 : GL_RGB16;
		}

		assert(dataSet->getData() != NULL);
		assert(sourceType != 0);
		assert(internalFormat != 0);

		glTexImage3D(GL_TEXTURE_3D,0,internalFormat,
			dataSet->getWidth(),
			dataSet->getHeight(),
			dataSet->getDepth(),
			0,format,sourceType,dataSet->getData());
	}
}
