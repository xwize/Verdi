
#include "VerdiPrerequisites.h"

// We also need GL, GLU and GLee.
#include <GLee.h>

namespace Verdi {

	void _glAssert(const char* szMessage, const char* szFile, int iLine)
	{
		static int iPrevLine = -1;
		static std::string strPrevFile = "";

		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			std::string strErrorString = "";
			switch(error)
			{
			case GL_INVALID_ENUM : 
				strErrorString = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE : 
				strErrorString = "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION : 
				strErrorString = "GL_INVALID_OPERATION"; break;
			default : 
				strErrorString = "UNKNOWN ERROR"; break;
			};

			std::cout << strErrorString << " @ " << szFile
				<< ":" << iLine << " <- " 
				<< strPrevFile << ":" << iPrevLine << " | " << szMessage;	
		}

		assert(error == GL_NO_ERROR);

		iPrevLine = iLine;
		strPrevFile = szFile;
	}
}