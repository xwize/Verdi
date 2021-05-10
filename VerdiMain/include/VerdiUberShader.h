
#ifndef __Verdi_Uber_Shader_H__
#define __Verdi_Uber_Shader_H__

#include "VerdiPrerequisites.h"
#include "VerdiVec4.h"
#include "VerdiVec3.h"

namespace Verdi {

	/** An interface for implementations to provide
		shader support. An uber-shader is a high-level
		shader that contains multiple versions of the same
		shader - each of which uses define flags to enable
		and disable certain functionality.
	*/
	class UberShader
	{
	public:
		// Contains the name and the corresponding define
		// strings for a particular instance of effect
		struct EffectDescriptor {
			uint32_t identifier;
			StringVector defines;
		};
		// A vector of effect descriptors
		typedef std::vector<EffectDescriptor> EffectDescriptorVector;
		// A handle for parameters
		typedef uint32_t Parameter;
	public:

		/** Bind the chosen effect with the given pass number.
		*/
		virtual void bind(int passNumber) = 0;

		/** Unbind the uber shader.
		*/
		virtual void unbind() = 0;

		/** Load the uber-shader.
		*/
		virtual void load() = 0;

		/** Unload the uber-shader.
		*/
		virtual void unload() = 0;

		/** Select an effect to use.
		*/
		virtual void useEffect(uint32_t identifier) = 0;

		/** Get a semantic parameter.
		*/
		virtual Parameter getSemanticParameter(const String& name) = 0;

		/** Set a parameter from a 4d vector.
		*/
		virtual void setVectorParameter(Parameter p, const Vec4& v) = 0;

		/** Set a parameter from a 3d vector.
		*/
		virtual void setVectorParameter(Parameter p, const Vec3& v) = 0;

		/** Set a scalar parameter.
		*/
		virtual void setScalarParameter(Parameter p, float s) = 0;

		/** Set a scalar parameter.
		*/
		virtual void setScalarParameter(Parameter p, int s) = 0;

		/** Set a texture parameter.
		*/
		virtual void setTextureParameter(Parameter p, int s) = 0;

		/** Set the a matrix parameter from OpenGL.
		*/
		virtual void setModelViewParameter(Parameter p) = 0;

		/** Set the a matrix parameter from OpenGL.
		*/
		virtual void setModelViewProjectionParameter(Parameter p) = 0;
	};
}

#endif // __Verdi_Uber_Shader_H__