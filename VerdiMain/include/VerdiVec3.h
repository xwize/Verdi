
#ifndef __Verdi_Vec3_H__
#define __Verdi_Vec3_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec4.h"

namespace Verdi {

	/** A class for 3d vector operations.
	@remarks
		This class does not default to (0.0,0.0,0.0)
		as one would expect. It should be set
		explicitly using the parameterised constructor.
	*/
	class Vec3
	{
	public:
		union {
			struct {
				float x;
				float y;
				float z;
			};
			float values[3];
		};
	public:

		/** Default constructor.
		*/
		Vec3()
		{
		}

		/** Parameterised constructor.
		*/
		Vec3(float ix, float iy, float iz) : x(ix), y(iy), z(iz)
		{
		}

		/** Copy constructor for 4d vector.
		*/
		Vec3(const Vec4& v) : x(v.x), y(v.y), z(v.z)
		{

		}

		/** Default destructor.
		*/
		~Vec3()
		{
		}

		/** Equality operator.
		*/
		bool operator == (const Vec3& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}

		/** Addition operator.
		*/
		Vec3 operator + (const Vec3& v) const
		{
			return Vec3(x+v.x,y+v.y,z+v.z);
		}

		/** Addition with a scalar.
		*/
		Vec3 operator + (float a) const
		{
			return Vec3(x+a, y+a, z+a);
		}

		/** Plus equals operator.
		*/
		Vec3 operator += (const Vec3& v)
		{
			x += v.x; y += v.y; z += v.z;
			return (*this);
		}
		
		/** Plus equals with a scalar.
		*/
		Vec3 operator += (float a)
		{
			x += a; y += a; z += a;
			return (*this);
		}

		/** Times equals operator.
		*/
		Vec3 operator *= (float s) 
		{
			x *= s; y *= s; z *= s;
			return (*this);
		}

		/** Multiplication operator.
		*/
		Vec3 operator * (float s) const 
		{
			return Vec3(x*s,y*s,z*s);
		}

		/** Multiplication operator.
		*/
		Vec3 operator * (const Vec3& v) const 
		{
			return Vec3(x*v.x,y*v.y,z*v.z);
		}

		/** Times equals operator.
		*/
		Vec3 operator *= (const Vec3& v)
		{
			x *= v.x; y *= v.y; z *= v.z;
			return (*this);
		}

		/** Subtraction operator.
		*/
		Vec3 operator - (const Vec3& v) const
		{
			return Vec3(x-v.x,y-v.y,z-v.z);
		}

		/** Performs a dot product.
		*/
		float dot(const Vec3& v) const
		{
			return x*v.x + y*v.y + z*v.z;
		}

		/** Returns 1 over v.
		*/
		Vec3 inverse() const 
		{
			return Vec3(1.0f/x,1.0f/y,1.0f/z);
		}

		/** Performs a cross product.
		*/
		Vec3 cross(const Vec3& v) const
		{
			Vec3 r;
			r.x = y*v.z - z*v.y;
			r.y = z*v.x - x*v.z;
			r.z = x*v.y - y*v.x;
			return r;
		}

		/** Returns the angle between two vectors
			in radians.
		*/
		Radian angleBetween(const Vec3& v) const
		{
			return acosf(dot(v)/(length()*v.length()));
		}

		/** Get the length of the vector squared.
		@remarks
			Should be used in cases when the exact
			value is not required.
		*/	
		float lengthSquared() const
		{
			return x*x + y*y + z*z;
		}

		float length() const
		{
			return sqrtf( x*x + y*y + z*z );
		}

		void normalise()
		{
			// Make the vector unit length by dividing
			// all elements by the length
			float ilen = 1.0f/length();
			x *= ilen;
			y *= ilen;
			z *= ilen;
		}
	};
}

#endif // __Verdi_Vec3_H__