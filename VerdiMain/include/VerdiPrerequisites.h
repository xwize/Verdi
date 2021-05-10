
#ifndef __Verdi_Prerequisites_H__
#define __Verdi_Prerequisites_H__

/** Visual studio complains a lot when using
	"deprecated" libraries to try to get you
	to use _s "safe" versions which are
	non-standard. We'll see about that.
*/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

/** Visual studio says anonymous structs 
	aren't part of the standard. News to me
	as this code compiles with both g++ and 
	MSVC.
*/
#ifdef _MSC_VER
#pragma warning( disable : 4201 )
#endif

// Standard library includes
#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Version tracking
#include "VerdiVersion.h"

// For debugging
#ifdef _DEBUG
#define VERDI_LOG(x) (std::cout << "Verdi (" << typeid(*this).name() << "): " << x << std::endl)
#else
#define VERDI_LOG(x)
#endif

#ifndef VERDI_GL_OK
#define VERDI_GL_OK() Verdi::_glAssert("",__FILE__,__LINE__);
#endif

namespace Verdi {

	// Useful typedefs
	typedef std::string String;
	typedef std::vector<std::string> StringVector;
	typedef std::map<std::string, std::string> StringMap;

	typedef float Radian;
	typedef float Degree;

	// Forward declarations
	class DataSet;
	class HardwareDataSet;
	class HardwareTransferFunction;
	class RenderContext;
	class RenderStrategy;
	class RenderSystem;
	class TransferFunction;

	// Helpful OpenGL error tracking
	void _glAssert(const char* szMessage, const char* szFile, int iLine);
}

#endif // __Verdi_Prerequisites_H__
