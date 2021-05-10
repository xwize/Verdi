
#ifndef __Verdi_Mat4_H__
#define __Verdi_Mat4_H__

#include "VerdiMath.h"
#include "VerdiVec3.h"

namespace Verdi {

	/** A class for storing a 4x4 homogeneous matrix.
	*/
	class Mat4
	{
	public:
		union
		{
			float values[16];
			struct  
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
		};
	public:

		Mat4()
		{

		}

		~Mat4()
		{

		}

		Mat4& operator = (const Mat4& m) 
		{
			// Simply copy the values
			for(size_t i = 0; i < 16; ++i)
				values[i] = m.values[i];
			return (*this);
		}

		Vec4 operator * (const Vec3& v) const
		{
			Vec4 ret;
			ret.x = _11*v.x + _21*v.y + _31*v.z + _41;
			ret.y = _12*v.x + _22*v.y + _32*v.z + _42;
			ret.z = _13*v.x + _23*v.y + _33*v.z + _43;
			ret.w = _14*v.x + _24*v.y + _34*v.z + _44;
			return ret;
		}

		Mat4 operator * (const Mat4& m) const
		{
			Mat4 r;

			// Rotate the right vector by m
			r._11 = _11 * m._11 + _12*m._21 + _13*m._31 + _14*m._41;
			r._12 = _11 * m._12 + _12*m._22 + _13*m._32 + _14*m._42;
			r._13 = _11 * m._13 + _12*m._23 + _13*m._33 + _14*m._43;
			r._14 = _11 * m._14 + _12*m._24 + _13*m._34 + _14*m._44;

			// Rotate the up vector by m
			r._21 = _21 * m._11 + _22*m._21 + _23*m._31 + _24*m._41;
			r._22 = _21 * m._12 + _22*m._22 + _23*m._32 + _24*m._42;
			r._23 = _21 * m._13 + _22*m._23 + _23*m._33 + _24*m._43;
			r._24 = _21 * m._14 + _22*m._24 + _23*m._34 + _24*m._44;

			// Rotate the forward vector by m
			r._31 = _31 * m._11 + _32*m._21 + _33*m._31 + _34*m._41;
			r._32 = _31 * m._12 + _32*m._22 + _33*m._32 + _34*m._42;
			r._33 = _31 * m._13 + _32*m._23 + _33*m._33 + _34*m._43;
			r._34 = _31 * m._14 + _32*m._24 + _33*m._34 + _34*m._44;

			// Rotate the homogeneous translation vector by m
			r._41 = _41 * m._11 + _42*m._21 + _43*m._31 + _44*m._41;
			r._42 = _41 * m._12 + _42*m._22 + _43*m._32 + _44*m._42;
			r._43 = _41 * m._13 + _42*m._23 + _43*m._33 + _44*m._43;
			r._44 = _41 * m._14 + _42*m._24 + _43*m._34 + _44*m._44;

			return r;
		}

		Mat4 getTranspose()
		{
			/** 
			 * A transpose simply just flips the rows and columns.
			 * We let [i][j] become [j][i]...
			 **/
			Mat4 mat;

			for(int i = 0; i < 4; ++i)
			{
				for(int j = 0; j < 4; ++j)
					mat.values[i+j*4] = values[j+i*4];
			}

			return mat;
		}

		Mat4 getOrthonormalInverse() const
		{
			// Copy the negated translation components
			Mat4 transComp;
			transComp.setIdentity();
			transComp.setTranslation(-_41,-_42,-_43);

			// Copy and transpose rotation components
			Mat4 rotComp;
			rotComp = (*this);
			rotComp.setTranslation(0.0f,0.0f,0.0f);
			rotComp = rotComp.getTranspose();

			// Multiply
			Mat4 invMat = transComp * rotComp;
			return invMat;
		}

		void setIdentity()
		{
			// A scaling matrix with unit scale
			_11 = 1; _12 = 0; _13 = 0; _14 = 0;
			_21 = 0; _22 = 1; _23 = 0; _24 = 0;
			_31 = 0; _32 = 0; _33 = 1; _34 = 0;
			_41 = 0; _42 = 0; _43 = 0; _44 = 1;
		}

		void setRotationX(Radian radians)
		{
			float sr = sinf(radians);
			float cr = cosf(radians);

			_22 = cr; _23 = -sr;
			_32 = sr; _33 = cr;
		}

		void setRotationY(Radian radians)
		{
			float sr = sinf(radians);
			float cr = cosf(radians);

			_11 = cr; _13 = sr;
			_31 = -sr; _33 = cr;
		}

		void setRotationZ(Radian radians)
		{
			float sr = sinf(radians);
			float cr = cosf(radians);

			_11 = cr; _12 = -sr;
			_21 = sr; _22 = cr;
		}

		void setScale(float x, float y, float z)
		{
			_11 = x; _22 = y; _33 = z;
		}

		void setTranslation(float x, float y, float z)
		{
			_41 = x; _42 = y; _43 = z;
		}
	};
}
#endif // __Verdi_Mat4_H__