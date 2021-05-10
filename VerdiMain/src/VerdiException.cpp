
#include "VerdiException.h"

namespace Verdi {

	Exception::Exception()
	{

	}

	Exception::Exception(const Exception& e)
	{
		mCode = e.getCode();
		mDescription = e.getDescription();
		mLine = e.getLine();
		mSourceFile = e.getSourceFile();
	}
}