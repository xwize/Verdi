
#ifndef __Verdi_Compositor_H__
#define __Verdi_Compositor_H__

#include "VerdiPrerequisites.h"

namespace Verdi {

	/** Class representing a compositing mode.
	*/
	class Compositor
	{
	public:
		/** Available compositing modes.
		*/
		enum CompositingMode {
			CCM_MIP,
			CCM_EMISSION_ABSORPTION
		};
	protected:
		CompositingMode mCompositingMode;
	public:

		/** Default constructor.
		*/
		Compositor(CompositingMode mode) : 
		  mCompositingMode(mode) {}

		/** Returns the compositing mode.
		*/
		CompositingMode getCompositingMode() const { return mCompositingMode; }

		/** Set the compositing mode.
		*/
		void setCompositingMode(CompositingMode mode) { mCompositingMode = mode; }

	};
}
#endif // __Verdi_Compositor_H__