
#ifndef __Verdi_Ray_Cast_Strategy_Impl_H__
#define __Verdi_Ray_Cast_Strategy_Impl_H__

// Include the framework
#include "Verdi.h"
#include "VerdiDefaultRenderStrategy.h"

namespace Verdi {

	/** RayCastStrategy implementation.
	*/
	class RayCastStrategy : public DefaultRenderStrategy
	{
	protected:
		GLuint mRenderTextureWidth;
		GLuint mRenderTextureHeight;

		GLuint mFrameBufferObject;
		GLuint mFinalRenderTexture;
		GLuint mFaceRenderBufferObject[2];
		GLuint mFaceRenderTexture[2];

		static const int BACK_FACE_TEXTURE = 0;
		static const int FRONT_FACE_TEXTURE = 1;

		static const int POSITION_PASS = 0;
		static const int NEAR_PLANE_PASS = 1;
		static const int RAY_CASTING_PASS = 2;

		void _getEffectDescriptorFromParameters(bool onTheFlyGradients, 
			bool useErt, bool useLighting, bool useLMIP, 
			UberShader::EffectDescriptor& effectOut) const;

		bool _recurseMinMaxOctree( MinMaxOctree::Node* parent, bool onlyHalf );
		void _drawMinMaxOctreeNode( MinMaxOctree::Node* node );
		void _setupShaders();
		void _setupRenderTextures(GLuint w, GLuint h);
		void _submitProxyGeometry(bool onlyHalf = false);
		void _submitNearPlane();
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
		RayCastStrategy(const String& shaderPath = "shaders/RayCastStrategy/RayCast.cgfx");

		/** Default destructor.
		*/
		~RayCastStrategy();
	};
}

#endif // __Verdi_Ray_Cast_Strategy_Impl_H__
