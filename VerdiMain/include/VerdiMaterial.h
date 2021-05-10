
#ifndef __Verdi_Material_H__
#define __Verdi_Material_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec4.h"

namespace Verdi {

	/** A simple class for storing material
		properties.
	*/
	class Material
	{
	public:
		Vec4	ambient;
		Vec4	diffuse;
		Vec4	specular;
		float	specularExponent;
	public:

		/** Constructor.
		*/
		Material() : ambient(0.2f,0.2f,0.2f,1.0f), 
			diffuse(0.5f,0.5f,0.5f,1.0f), specular(0.5f,0.5f,0.5f,1.0f),
			specularExponent(1.0f)
		{

		}
	};
}

#endif // __Verdi_Material_H__