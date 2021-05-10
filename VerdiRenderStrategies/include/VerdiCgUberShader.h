
#ifndef __Verdi_Cg_Uber_Shader_H__
#define __Verdi_Cg_Uber_Shader_H__

// Requires Verdi
#include "VerdiPrerequisites.h"
#include "VerdiUberShader.h"

// Requires Cg
#include <Cg/cg.h>
#include <Cg/cgGL.h>

namespace Verdi {

	/** A wrapper around a Cg effect that enables
		"uber shader" functionality. An uber shader
		is commonly a shader that contains preprocessor
		statements to create multiple shaders from a single
		file. This comes in handy, when shaders that all
		use the same algorithm but have subtle variations
		(e.g. 2 lights vs 3 lights) need to be managed
		effectively.
	*/
	class CgUberShader : public Verdi::UberShader
	{
	public:
		typedef std::map<uint32_t,CGeffect> EffectIdentifierMap;
	protected:
		void _checkForCgError(const String& errorMessage);
	protected:
		static CGcontext mContext;
		static int mReferenceCount;
		String mSourceFile;
		uint32_t mCurrentEffectIdentifier;
		CGpass mCurrentPass;
		EffectIdentifierMap mEffects;
		EffectDescriptorVector mEffectDescriptors;
	public:

		/** Parameterised constructor.
		*/
		CgUberShader(const String& sourceFile,
			const EffectDescriptorVector& effects);

		/** Default destructor.
		*/
		~CgUberShader();

		/** Perform loading.
		*/
		void load();

		/** Cleanup.
		*/
		void unload();

		/** Use a particular effect.
		*/
		void useEffect(uint32_t id);

		/** Bind a particular pass.
		*/
		void bind(int passNumber);

		/** Unbind.
		*/
		void unbind();

		/** Gets the effect assigned to the given name.
		*/
		CGeffect getEffect(uint32_t id) const;

		/** Gets shader semantic for the currently bound effect.
		*/
		Parameter getSemanticParameter(const String& name);

		/** Set a parameter from a 4d vector.
		*/
		void setVectorParameter(Parameter p, const Vec4& v);

		/** Set a parameter from a 3d vector.
		*/
		void setVectorParameter(Parameter p, const Vec3& v);

		/** Set a scalar parameter.
		*/
		void setScalarParameter(Parameter p, float s);

		/** Set a scalar parameter.
		*/
		void setScalarParameter(Parameter p, int s);

		/** Set a texture parameter.
		*/
		void setTextureParameter(Parameter p, int s);

		/** Set the a matrix parameter from OpenGL.
		*/
		void setModelViewParameter(Parameter p);

		/** Set the a matrix parameter from OpenGL.
		*/
		void setModelViewProjectionParameter(Parameter p);
	};
}

#endif // __Verdi_Cg_Uber_Shader_H__