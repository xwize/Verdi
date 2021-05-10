
#ifndef __Verdi_Cosine_Spline_H__
#define __Verdi_Cosine_Spline_H__

#include "VerdiSpline.h"

namespace Verdi {

	/** Class for implementing cosine splines.
		Based on the cosine interpolation function.
	*/
	class CosineSpline : public Spline
	{
	protected:
	public:

		/** Sample the spline.
		*/
		Vec4 sample(float x) const;
	};
}

#endif // __Verdi_Cosine_Spline_H__