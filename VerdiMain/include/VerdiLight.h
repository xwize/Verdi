
#ifndef __Verdi_Light_H__
#define __Verdi_Light_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec3.h"

namespace Verdi {

	/** A simple class for storing
		a full-bright point light.
	*/
	class Light
	{
	public:
		Vec3	position;
		float	radius;
	public:

		/** Constructor.
		*/
		Light() : position(0.0f,1.0f,0.0f), radius(50.0f)
		{
		}
	};
}

#endif // __Verdi_Light_H__