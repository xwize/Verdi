
#ifndef __Verdi_Math_H__
#define __Verdi_Math_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec4.h"
#include "VerdiVec3.h"

namespace Verdi {

	/** This class contains useful
		math operations and constants that aren't 
		provided with the standard libraries.
	*/
	class Math
	{
	protected:
	public:

		// Useful constants
		static const float PI;
		static const float PI2;
		static const float INFINITY_POSITIVE;
		static const float INFINITY_NEGATIVE;
		static const float EPSILON;

		// Linear interpolation
		static float linearInterpolate(float a, float b, float t)
		{
			return (1.0f-t)*a + t*b;
		}

		// Linear interpolation on Vec4
		static Vec4 linearInterpolate(const Vec4& a, const Vec4& b, float t)
		{
			Vec4 r;
			r.x = linearInterpolate(a.x,b.x,t);
			r.y = linearInterpolate(a.y,b.y,t);
			r.z = linearInterpolate(a.z,b.z,t);
			r.w = linearInterpolate(a.w,b.w,t);
			return r;

		}

		// Cosine interpolation
		static float cosineInterpolate(float a, float b, float t)
		{
			t = (1.0f-cosf(t*PI))*0.5f;
			return linearInterpolate(a,b,t);
		}
		
		// Quadratic Bezier interpolation
		static float quadraticBezier(float a, float b, float c, float t)
		{
			return linearInterpolate(linearInterpolate(a,b,t), linearInterpolate(b,c,t), t);
		}

		// Cubic Bezier interpolation
		static float cubicBezier(float a, float b, float c, float d, float t)
		{
			return linearInterpolate(quadraticBezier(a,b,c,t),quadraticBezier(b,c,d,t),t);
		}

		// Max
		template<class T> static T maximum(const T& a, const T& b) 
		{
			return (b > a ? b : a);
		}

		// Min
		template<class T> static T minimum(const T& a, const T& b) 
		{
			return (b < a ? b : a);
		}

		// Clamp
		template<class T> static T clamp(const T& v, const T& a, const T& b)
		{
			return maximum<T>(a,minimum<T>(v,b)); 
		}

		// Conversion from spherical coordinates
		static Vec3 fromSpherical(Radian yaw, Radian pitch) 
		{
			/*Vec3 ret;
			ret.x = cosf(yaw + Verdi::Math::PI/2.0f)*cosf(pitch);
			ret.y = sinf(pitch);
			ret.z = sinf(yaw + Verdi::Math::PI/2.0f)*cosf(pitch);*/

			Vec3 ret;
			ret.x = cos(yaw) * sin(pitch);
			ret.z = sin(yaw) * sin(pitch);
			ret.y = cos(pitch);

			return ret;
		}

		// Conversion to spherical coordinates
		static void toSpherical(const Vec3& direction, Radian* yaw, Radian* pitch)
		{
			Vec3 nd = direction;
			nd.normalise();

			float lnd = direction.length();

			Vec3 fwd = Vec3(0.0f,0.0f,1.0f);
			Vec3 right = Vec3(1.0f,0.0f,0.0f);

			if(lnd == 0.0f)
				return;

			(*pitch) = acosf(direction.y/lnd);
			(*yaw) = atan2f(direction.z,direction.x);
		}
	};
}

#endif // __Verdi_Math_H__