
#ifndef __Verdi_Pvm_Reader_H__
#define __Verdi_Pvm_Reader_H__

#include "VerdiDataSet.h"
#include "VerdiDataSetReader.h"

namespace Verdi {

	/** Concrete implementation of a DataSet reader
		for loading pvm voxel files.
	*/
	class PvmReader : public DataSetReader
	{
	protected:
	public:

		/** Read a data set from a file.
		@param filename The filename of the file to load.
		@param args Optional arguments encoded as a string map.
		@returns Returns a pointer to the loaded data set.
		*/
		DataSet* readFromFile(const String& filename,
			const StringMap* args);
	};
}

#endif // __Verdi_Pvm_Reader_H__