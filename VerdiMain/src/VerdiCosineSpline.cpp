
#include "VerdiCosineSpline.h"
#include "VerdiException.h"

namespace Verdi {

	Vec4 CosineSpline::sample(float x) const 
	{
		int numControlPoints = (int)mControlPoints.size();

		if(numControlPoints < 2)
		{
			VERDI_THROW(Verdi::StateInvalidException,"Minimum of 2 control points required.",0);
		}

		ControlPoint dummyLeft(0.0f,mControlPoints[0].second);
		ControlPoint dummyRight(1.0f,mControlPoints[numControlPoints-1].second);

		const ControlPoint* points[4] = {&dummyLeft, &dummyLeft, &dummyRight, &dummyRight};
		
		for(int i = 0; i < numControlPoints; ++i)
		{
			if(mControlPoints[i].first > x)
			{
				if(i-2 >= 0)
					points[0] = &mControlPoints[i-2];
				
				if(i-1 >= 0)
					points[1] = &mControlPoints[i-1];

				if(i+1 < numControlPoints)
					points[3] = &mControlPoints[i+1];

				points[2] = &mControlPoints[i];
				break;
			}
		}

		float unitised = (x - points[1]->first)/(points[2]->first - points[1]->first);

		Vec4 val;
		val.x = Math::cosineInterpolate(points[1]->second.x,points[2]->second.x,unitised);
		val.y = Math::cosineInterpolate(points[1]->second.y,points[2]->second.y,unitised);
		val.z = Math::cosineInterpolate(points[1]->second.z,points[2]->second.z,unitised);
		val.w = Math::cosineInterpolate(points[1]->second.w,points[2]->second.w,unitised);
		return val;
	}
}