
#ifndef __Verdi_Vec4_H__
#define __Verdi_Vec4_H__

#include "VerdiPrerequisites.h"

namespace Verdi {

	/** Class definition for a 4d vector.
	*/
	class Vec4
	{
	public:
		union {
			struct {
				float x;
				float y;
				float z;
				float w;
			};
			float values[4];
		};
	public:

		/** Default constructor.
		*/
		Vec4()
		{
		}

		/** Parameterised constructor.
		*/
		Vec4(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw)
		{
		}

		/** Default destructor.
		*/
		~Vec4()
		{
		}

		/** Subtraction operator.
		*/
		Vec4 operator - (const Vec4& v) const
		{
			return Vec4(x-v.x,y-v.y,z-v.z,w-v.w);
		}

		/** Times equals operator.
		*/
		Vec4 operator *= (float s) 
		{
			x *= s; y *= s; z *= s; w *= s;
			return (*this);
		}

		/** Get the length of the vector squared.
		@remarks
			Should be used in cases when the exact
			value is not required.
		*/	
		float lengthSquared() const
		{
			return x*x + y*y + z*z + w*w;
		}

		float length() const
		{
			return sqrtf( x*x + y*y + z*z + w*w );
		}
	};
}

#endif // __Verdi_Vec4_H__