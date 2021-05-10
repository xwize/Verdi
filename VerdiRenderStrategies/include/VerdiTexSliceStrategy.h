
#ifndef __Verdi_Tex_Slice_Strategy_Impl_H__
#define __Verdi_Tex_Slice_Strategy_Impl_H__

// Include the framework
#include "Verdi.h"
#include "VerdiDefaultRenderStrategy.h"

namespace Verdi {

	/** TexSliceStrategy implementation.
	*/
	class TexSliceStrategy : public DefaultRenderStrategy
	{
	protected:
	protected:
		GLuint mRenderTextureWidth;
		GLuint mRenderTextureHeight;

		GLuint mFrameBufferObject[2];
		GLuint mFrameRenderBufferObject[2];
		GLuint mFinalRenderTexture[2];

	protected:

		void _getEffectDescriptorFromParameters(bool onTheFlyGradients, bool useLighting,  
			UberShader::EffectDescriptor& effectOut) const;

		void _submitBlockSlice(const Vec3& offset, const Vec3& scale, int planeIndex);
		void _setupShaders();
		void _setupRenderTextures(GLuint w, GLuint h);
		void _submitPolygonSlice(int planeIndex);
		int _computeFrontIndex();
	protected:
		void _beginFrameImpl(float dt, float time);
		void _renderImpl();
		void _endFrameImpl();
		void _initialiseImpl(uint32_t w, uint32_t h);
		void _shutdownImpl();
		void _resizeImpl(uint32_t width, uint32_t height);
	public:

		/** Default constructor.
		*/
		TexSliceStrategy(const String& shaderPath = "shaders/RayCastStrategy/TexSlice.cgfx");

		/** Default destructor.
		*/
		~TexSliceStrategy();
	};
}

#endif // __Verdi_Tex_Slice_Strategy_Impl_H__