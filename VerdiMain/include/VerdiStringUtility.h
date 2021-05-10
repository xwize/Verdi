
#ifndef __Verdi_String_Utility_H__
#define __Verdi_String_Utility_H__

#include "VerdiPrerequisites.h"
#include "VerdiException.h"
#include "VerdiVec3.h"
#include "VerdiVec4.h"

namespace Verdi {

	class StringUtility
	{
	protected:
	public:

		/** Convert an exception to a string.
		*/
		static String toString(const Exception& e);

		/** Convert an integer to a string.
		*/
		static String toString(int a);

		/** Convert an unsigned integer to a string.
		*/
		static String toString(uint32_t a);

		/** Convert a float to a string.
		*/
		static String toString(float a);

		/** Convert a 4d vector to a string.
		*/
		static String toString(const Vec4& v);

		/** Convert a 3d vector to a string.
		*/
		static String toString(const Vec3& v);

		/** Convert a string to 3d vector.
		*/
		static Vec3 toVec3(const String& str);

		/** Convert a string to 4d vector.
		*/
		static Vec4 toVec4(const String& str);

		/** Convert a string to an integer.
		*/
		static int toInt(const String& str);

		/** Convert a string to a float.
		*/
		static float toFloat(const String& str);

		/** Split the string based on a delimiter
			into a list of strings. Delimiter is 
			kept at the end of the string.
		*/
		static void split(const String& str, const char delim, StringVector& sub);
	};
}

#endif // __Verdi_String_Utility_H__