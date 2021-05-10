
#include "VerdiHardwareTransferFunction.h"
#include "VerdiException.h"

// We also need GL, GLU and GLee.
#include <GLee.h>

namespace Verdi {

	HardwareTransferFunction::~HardwareTransferFunction()
	{
		VERDI_LOG("Destructor called.");
		clear();
	}

	void HardwareTransferFunction::clear()
	{
		if(mHardwareId != 0)
		{
			glDeleteTextures(1,&mHardwareId);
			mHardwareId = 0;
		}
	}

	void HardwareTransferFunction::updateFromTransferFunction(
		const TransferFunction* transferFunction)
	{
		if(transferFunction == NULL)
			VERDI_THROW(Verdi::ParameterInvalidException,
				"Parameter transferFunction must not be NULL.",0);

		int32_t format = GL_RGBA;
		int32_t sourceType = GL_UNSIGNED_SHORT;
		int32_t internalFormat = GL_RGBA16;

		// First create the texture if we haven't
		// done so already
		if(mHardwareId == 0)
		{
			glGenTextures(1,&mHardwareId);
			glBindTexture(GL_TEXTURE_2D,mHardwareId);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
			

			glTexImage2D(GL_TEXTURE_2D,0,internalFormat,
				transferFunction->getWidth(),
				transferFunction->getHeight(),
				0,format,sourceType,transferFunction->getData());

			return;
		}

		// Otherwise update
		glBindTexture(GL_TEXTURE_2D,mHardwareId);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,
			transferFunction->getWidth(),
			transferFunction->getHeight(),
			format,sourceType,transferFunction->getData());
	}
}
