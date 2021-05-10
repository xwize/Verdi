
#include "VerdiRawReader.h"
#include "VerdiException.h"
#include "VerdiStringUtility.h"

namespace Verdi {

	DataSet* RawReader::readFromFile(const String& filename, const StringMap* args)
	{
		if(args == NULL)
			VERDI_THROW(Verdi::ParameterInvalidException,"Value of args cannot be NULL",0);

		int w = 0;
		int h = 0;
		int d = 0;
		int c = 1;

		DataSet::ChannelFormat format = DataSet::DSCF_UBYTE8;

		StringMap::const_iterator i;
		
		i = args->find("width");
		if( i != args->end() ) w = StringUtility::toInt(i->second);

		i = args->find("height");
		if( i != args->end() ) h = StringUtility::toInt(i->second);

		i = args->find("depth");
		if( i != args->end() ) d = StringUtility::toInt(i->second);

		i = args->find("components");
		if( i != args->end() ) c = StringUtility::toInt(i->second);

		i = args->find("format");
		if( i != args->end() ) format = (DataSet::ChannelFormat)StringUtility::toInt(i->second);

		if(w <= 0 || h <= 0 || d <= 0 || c <= 0)
			VERDI_THROW(Verdi::ParameterInvalidException,"Width, height, depth and components must be positive and non-zero",1);


		int numElements = w*h*d*c;
		int bytesPerElement = 1;
		int fileSize = 0;

		switch(format)
		{
		case DataSet::DSCF_UBYTE8 : bytesPerElement = 1; break;
		case DataSet::DSCF_UBYTE16 : bytesPerElement = 2; break;
		case DataSet::DSCF_UBYTE32 : bytesPerElement = 4; break;
		case DataSet::DSCF_FLOAT32 : bytesPerElement = 4; break;
		};

		fileSize = bytesPerElement*numElements;
		uint8_t* data = new uint8_t[fileSize];

		FILE* p = fopen(filename.c_str(),"rb");

		if(p == NULL)
		{
			delete [] data;
			VERDI_THROW(Verdi::FileNotFoundException,"File: " + filename + " not found",0);
		}

		fseek(p,0,SEEK_END);
		if( ftell(p) != fileSize )
		{
			int actual = ftell(p);
			fclose(p);
			delete [] data;

			String desc = "computed: " + StringUtility::toString(fileSize)
				+ ", actual: " + StringUtility::toString(actual);

			VERDI_THROW(Verdi::FileLoadFailedException,"Invalid file size: " + desc,0);
		}

		fseek(p,0,SEEK_SET);
		fread(data,1,fileSize,p);
		fclose(p);

		return new DataSet(w,h,d,c,format,data);
	}
}