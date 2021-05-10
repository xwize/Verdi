
#ifndef __Verdi_Spline_H__
#define __Verdi_Spline_H__

#include "VerdiMath.h"
#include "VerdiVec4.h"

namespace Verdi {

	/** A base class for representing splines.
	*/
	class Spline
	{
	public:
		class ControlPoint : public std::pair<float,Vec4>
		{
		public:
			ControlPoint(float x, const Vec4& v) 
			{
				this->first = x;
				this->second = v;
			}

			bool operator < (const ControlPoint& a) const 
			{
				return first < a.first;
			}
		};
		typedef std::vector<ControlPoint> ControlPointList;
	protected:
		ControlPointList mControlPoints;
	public:

		/** Default constructor.
		*/
		Spline();

		/** Clear all control points.
		*/
		virtual void clear();

		/** Add a control point.
		*/
		virtual void addControlPoint(const ControlPoint& point);

		/** Sample the spline at position x.
		*/
		virtual Vec4 sample(float x) const = 0;

		/** Calculates an estimate for the optimal
			resolution required to capture all the frequency
			components in this spline based on Nyquist sampling
			theory.
		*/
		virtual int getMinimalResolutionEstimate() const;
	};
}

#endif // __Verdi_Spline_H__