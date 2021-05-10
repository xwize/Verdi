
#include "VerdiSpline.h"

namespace Verdi {

	Spline::Spline()
	{
		mControlPoints.clear();
	}

	void Spline::clear()
	{
		mControlPoints.clear();
	}

	void Spline::addControlPoint(const ControlPoint& point)
	{
		mControlPoints.push_back(point);

		// Make sure it's sorted
		std::sort(mControlPoints.begin(),mControlPoints.end());
	}

	int Spline::getMinimalResolutionEstimate() const
	{
		// This is mostly some heuristics but there
		// is some theory involved.
		// First we take the minimum distance
		// between any two points on the x axis - d.
		// Then we take the gradient between these two points
		// and divide to get d / |grad| - which serves as
		// an estimate for how vast a value jumps - interpreted
		// as the period of a frequency component.
		// Our line is unit length, so we divide to get: |grad|/d.
		// We then multiply by 2 due to Nyquist.
		// Then we do 2^ceil(log2(...)) to round everything up
		// to the nearest power of 2.

		float smallestPeriod = Math::INFINITY_POSITIVE;

		for(size_t i = 0; i < mControlPoints.size()-1; ++i)
		{
			const ControlPoint& a = mControlPoints[i];
			const ControlPoint& b = mControlPoints[i+1];

			float dx = b.first - a.first;
			if(dx == 0.0f)
				continue;

			float dv = (b.second - a.second).length();
			if(dv == 0.0f)
				continue;

			float period = dx/dv;

			if(period < smallestPeriod)
				smallestPeriod = period;
		}

		int estimate = (int)powf(2.0f,ceilf(logf(2.0f/smallestPeriod)/logf(2.0f)));
		return estimate;
	}
}