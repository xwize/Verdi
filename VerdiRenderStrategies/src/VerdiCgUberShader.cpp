
#include "VerdiCgUberShader.h"
#include "VerdiException.h"
#include "VerdiStringUtility.h"

namespace Verdi {

	CGcontext CgUberShader::mContext = NULL;
	int CgUberShader::mReferenceCount = 0;

	CgUberShader::CgUberShader(const String& sourceFile, const EffectDescriptorVector& effects)
		: mSourceFile(sourceFile)
	{
		mCurrentEffectIdentifier = 0;
		mEffectDescriptors.clear();
		mEffects.clear();

		for(size_t i = 0; i < effects.size(); ++i) {
			mEffectDescriptors.push_back(effects[i]);
		}

		if(mReferenceCount <= 0 || mContext == NULL) {
			mReferenceCount = 0;
			VERDI_LOG("Creating CG context...");
			mContext = cgCreateContext();
			cgGLRegisterStates(mContext);
			cgGLSetManageTextureParameters(mContext, CG_TRUE);
		}

		mReferenceCount++;
	}

	CgUberShader::~CgUberShader()
	{
		VERDI_LOG("Destructor called.");
		unload();
		
		mReferenceCount--;

		if(mReferenceCount <= 0) {
			VERDI_LOG("Destroying CG context...");
			cgDestroyContext(mContext);
			mContext = NULL;
			mReferenceCount = 0;
		}
	}

	void CgUberShader::_checkForCgError(const String& errorMessage)
	{
		CGerror error;
		String errorDesc = errorMessage + " : ";

		VERDI_LOG("Checking for error...");

		// Get the last error string and error code
		const char* string = cgGetLastErrorString(&error);

		// If we have an error and a string
		if (error != CG_NO_ERROR && string != NULL) {

			// Append the string to the exception string
			errorDesc += String(string) + "\n";
			if (error == CG_COMPILER_ERROR) {
				// Get compiler messages
				errorDesc += String(cgGetLastListing(mContext));
			}

			// Reset the error state
			cgGetError(); 

			// Throw an exception
			VERDI_THROW(Verdi::FileLoadFailedException,errorDesc,0);
		} 
	}

	void CgUberShader::useEffect(uint32_t id) 
	{
		assert( mEffects.find(id) != mEffects.end() );
		mCurrentEffectIdentifier = id;
	}

	void CgUberShader::load()
	{
		assert(mContext != NULL);
		VERDI_LOG("Loading " << mSourceFile << "...");

		// Reset the error state
		cgGetError();

		mCurrentEffectIdentifier = 0;

		const int maxArgs = 256;
		const char* str[maxArgs]; 
		StringVector defineVector;

		VERDI_LOG("Compiling " << mEffectDescriptors.size() << " effects...");
		for(size_t i = 0; i < mEffectDescriptors.size(); ++i) {

			defineVector.clear();
			defineVector = mEffectDescriptors[i].defines;

			// Clear and set the define strings
			for(size_t j = 0; j < maxArgs; ++j) {
				str[j] = NULL;

				if(j < defineVector.size()) {
					str[j] = defineVector[j].c_str();
				}
			}

			// Perform loading
			CGeffect effect = cgCreateEffectFromFile(mContext,
				mSourceFile.c_str(), str );

			_checkForCgError("Loading effect " + mSourceFile + " : " + 
				Verdi::StringUtility::toString(mEffectDescriptors[i].identifier) );

			CGtechnique technique = cgGetFirstTechnique(effect);
			if(!cgValidateTechnique(technique)) {
				VERDI_LOG("Failed to validate first technique of " << mSourceFile);
				_checkForCgError("Validating first technique of " + mSourceFile + " : " +
					Verdi::StringUtility::toString(mEffectDescriptors[i].identifier) );
			}

			assert( mEffects.find(mEffectDescriptors[i].identifier) == mEffects.end() );
			mEffects[mEffectDescriptors[i].identifier] = effect;

			if(mCurrentEffectIdentifier == 0) {
				mCurrentEffectIdentifier = mEffectDescriptors[i].identifier;
				VERDI_LOG("Selecting effect " << mCurrentEffectIdentifier);
			}
		}
	}

	void CgUberShader::unload()
	{
		EffectIdentifierMap::iterator i = mEffects.begin();

		for( ; i != mEffects.end(); ++i)
			cgDestroyEffect(i->second);

		mEffects.clear();
	}

	CGeffect CgUberShader::getEffect(uint32_t id) const
	{
		EffectIdentifierMap::const_iterator i = mEffects.find(id);
		if( i == mEffects.end() )
			return 0;
		return i->second;
	}

	void CgUberShader::bind(int passNumber)
	{
		CGeffect effect = getEffect(mCurrentEffectIdentifier);
		CGtechnique firstTechnique = cgGetFirstTechnique(effect);

		CGpass pass = cgGetFirstPass(firstTechnique);

		int i = 0;
		while(i < passNumber) {
			pass = cgGetNextPass(pass);
			i++;
		}

		mCurrentPass = pass;
		cgSetPassState(mCurrentPass);
	}

	void CgUberShader::unbind()
	{
		cgResetPassState(mCurrentPass);
	}

	Verdi::UberShader::Parameter CgUberShader::getSemanticParameter(const String& name)
	{
		CGeffect effect = getEffect(mCurrentEffectIdentifier);
		CGparameter param = cgGetEffectParameterBySemantic( effect,name.c_str() );
		return (Parameter)param;
	}

	void CgUberShader::setVectorParameter( Parameter p, const Vec4& v )
	{
		cgGLSetParameter4fv((CGparameter)p,v.values);
	}

	void CgUberShader::setVectorParameter( Parameter p, const Vec3& v )
	{
		cgGLSetParameter3fv((CGparameter)p,v.values);
	}

	void CgUberShader::setModelViewParameter( Parameter p )
	{
		cgGLSetStateMatrixParameter((CGparameter)p,
			CG_GL_MODELVIEW_MATRIX,CG_GL_MATRIX_IDENTITY);	
	}

	void CgUberShader::setModelViewProjectionParameter( Parameter p )
	{
		cgGLSetStateMatrixParameter((CGparameter)p,
			CG_GL_MODELVIEW_PROJECTION_MATRIX,CG_GL_MATRIX_IDENTITY);	
	}

	void CgUberShader::setTextureParameter( Parameter p, int s )
	{
		cgGLSetTextureParameter((CGparameter)p,s);
	}

	void CgUberShader::setScalarParameter( Parameter p, int s )
	{
		cgGLSetParameter1d((CGparameter)p,s);
	}

	void CgUberShader::setScalarParameter( Parameter p, float s )
	{
		cgGLSetParameter1f((CGparameter)p,s);
	}

}