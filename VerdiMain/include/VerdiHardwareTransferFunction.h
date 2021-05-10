
#ifndef __Verdi_Hardware_Transfer_Function_H__
#define __Verdi_Hardware_Transfer_Function_H__

#include "VerdiPrerequisites.h"
#include "VerdiTransferFunction.h"

namespace Verdi {

	/** Class for representing transfer functions that have been
		converted to graphics textures.
	*/
	class HardwareTransferFunction
	{
	protected:
		uint32_t mHardwareId;
	public:

		/** Default constructor.
		*/
		HardwareTransferFunction() : mHardwareId(0) {};

		/** Default destructor.
		*/
		~HardwareTransferFunction();

		/** Clears the hardware texture.
		*/
		void clear();

		/** Updates the hardware texture with data
			from the supplied transfer function.
		*/
		void updateFromTransferFunction(const TransferFunction* transFunc);

		/** Returns the hardware texture identifier.
		*/
		uint32_t getHardwareIdentifier() const { return mHardwareId; }
	};
}

#endif // __Verdi_Hardware_Transfer_Function_H__