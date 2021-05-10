
#include "VerdiPvmReader.h"
#include "VerdiException.h"
#include "VerdiStringUtility.h"

#include "ddsbase/ddsbase.h"

namespace Verdi {

	DataSet* PvmReader::readFromFile(const String& filename, const StringMap* args)
	{
		// Set the data pointer to null
		unsigned char* volume = NULL;
		
		// Default values
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t depth = 0;
		uint32_t components = 0;
		
		DataSet::ChannelFormat format = DataSet::DSCF_UBYTE8;

		// Voxel scaling values
		float scalex;
		float scaley;
		float scalez;

		// Call the PVM reader
		volume = readPVMvolume(
			filename.c_str(),&width,&height,&depth,&components,&scalex,&scaley,&scalez);

		// If there was an error, free the memory and throw an exception
		if(volume == NULL || components == 0 ||
			width == 0 || height == 0 || depth == 0) {
			delete [] volume;
			VERDI_THROW(Verdi::FileLoadFailedException,"Failed to load " + filename,0);
		}

		
		// If the user has specified any argument strings
		if(args != NULL)
		{
			StringMap::const_iterator i;
			int quan = 0;

			// Try to match quantise
			i = args->find("quantise");
			if( i != args->end() ) 
				quan = StringUtility::toInt(i->second);

			// If it was enabled, and the volume is 16bits
			// quantise to 8bits 
			if(quan > 0 && components == 2)
			{
				volume = quantize(volume,width,height,depth,(quan-1)%2);
				components = 1;
			}
		}

		// Set the component format for the 
		// data set class to match
		switch(components)
		{
		case 1 : format = DataSet::DSCF_UBYTE8; break;
		case 2 : format = DataSet::DSCF_UBYTE16; break;
		case 4 : format = DataSet::DSCF_FLOAT32; break;
		default : { 
			delete [] volume; 
			VERDI_THROW(Verdi::FileLoadFailedException,"Invalid format when loading " + filename,0);
			}
		};

		// Construct a new data set and return it
		DataSet* dataSet = new DataSet(width,height,depth,1,format,volume);
		Vec3 scalingFactor(scalex,scaley,scalez);
		scalingFactor.normalise();
		dataSet->setScaleMultiplier(scalingFactor);
		return dataSet;
	}
}