
#ifndef __Verdi_Data_Set_Reader_H__
#define __Verdi_Data_Set_Reader_H__

#include "VerdiPrerequisites.h"
#include "VerdiDataSet.h"

namespace Verdi {

	/** Interface for readers that provide
		file loading functionality for data sets.
	*/
	class DataSetReader
	{
	protected:
	public:

		/** Read a data set from a file.
		@param filename The filename of the file to load.
		@param args Optional arguments encoded as a string map.
		@returns Returns a pointer to the loaded data set.
		*/
		virtual DataSet* readFromFile(const String& filename,
			const StringMap* args) = 0;
	};

	/** Auto ptr for freeing readers when end of scope reached.
	*/
	typedef std::auto_ptr<DataSetReader> DataSetReaderPtr;
}

#endif // __Verdi_Data_Set_Reader_H__