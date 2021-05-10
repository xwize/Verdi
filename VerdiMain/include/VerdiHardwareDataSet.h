
#ifndef __Verdi_Hardware_Data_Set_H__
#define __Verdi_Hardware_Data_Set_H__

#include "VerdiPrerequisites.h"
#include "VerdiDataSet.h"

namespace Verdi {

	/** Class for representing data sets that have been
		converted to graphics textures.
	*/
	class HardwareDataSet
	{
	protected:
		uint32_t mHardwareId;
	public:

		/** Default constructor.
		*/
		HardwareDataSet() : mHardwareId(0) {};

		/** From pre-existing constructor.
		*/
		HardwareDataSet(uint32_t hardwareId) : mHardwareId(hardwareId) {};

		/** Default destructor.
		*/
		~HardwareDataSet();

		/** Clears the hardware texture.
		*/
		void clear();

		/** Updates the hardware texture with data
			from the supplied dataSet.
		*/
		void updateFromDataSet(const DataSet* dataSet);

		/** Returns the hardware texture identifier.
		*/
		uint32_t getHardwareIdentifier() const { return mHardwareId; }
	};
}

#endif // __Verdi_Hardware_Data_Set_H__