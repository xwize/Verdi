
#ifndef __Verdi_Data_Set_Reader_Factory_H__
#define __Verdi_Data_Set_Reader_Factory_H__

#include "VerdiPrerequisites.h"
#include "VerdiException.h"
#include "VerdiDataSetReader.h"
#include "VerdiRawReader.h"
#include "VerdiPvmReader.h"

namespace Verdi {

	/** Class responsible for returning file readers based
		on the specified file format.
	*/
	class DataSetReaderFactory
	{
	public:
		/** Enum of supported file formats.
		*/
		enum DataSetReaderFormat {
			DSRF_RAW,
			DSRF_PVM
		};
	public:

		/** Returns a DataSetReader for a supplied format.
		@remarks
			An auto pointer is returned - lifetime
			control is passed to the caller.
		*/
		virtual DataSetReaderPtr getDataSetReader(DataSetReaderFormat format) const
		{
			switch(format)
			{
			case DSRF_RAW : return DataSetReaderPtr( new RawReader() ); break;
			case DSRF_PVM : return DataSetReaderPtr( new PvmReader() ); break;
			}

			VERDI_THROW(Verdi::ObjectNotFoundException,"DataSetReaderFormat unavailable",0);
			return DataSetReaderPtr(NULL);
		}
	};
}

#endif // __Verdi_Data_Set_Reader_Factory_H__