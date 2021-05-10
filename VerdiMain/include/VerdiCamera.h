
#ifndef __Verdi_Camera_H__
#define __Verdi_Camera_H__

#include "VerdiPrerequisites.h"
#include "VerdiMath.h"
#include "VerdiMat4.h"

namespace Verdi {

	/** A class for camera control.
	*/
	class Camera
	{
	public:
		enum ProjectionMode {
			CPM_PERSPECTIVE,
			CPM_ORTHOGRAPHIC
		};
	protected:
		float mNearDistance;
		float mFarDistance;
		Degree mFieldOfView;
		ProjectionMode mProjectionMode;
		Mat4 mTransform;
	public:

		/** Parameter constructor.
		*/
		Camera(float nearDist = 0.1f, float farDist = 50.0f, Degree fov = 45.0f);

		/** Set the camera matrix to the identity matrix.
		*/
		void setIdentity();

		/** Set the camera to look at a particular point.
		*/
		void lookAt(Vec3 position, Vec3 target, Vec3 upVector);

		/** Set the field of view in degrees.
		*/
		void setFieldOfView(Degree a) { mFieldOfView = a; }

		/** Set the minimum distance from the eye.
		*/
		void setNearDistance(float n) { mNearDistance = n; }

		/** Set the maximum distance from the eye.
		*/
		void setFarDistance(float f) { mFarDistance = f; }

		/** Set the view space transformation matrix.
		*/
		void setTransform(const Mat4& trans) { mTransform = trans; }

		/** Set the projection mode.
		*/
		void setProjectionMode(ProjectionMode mode) { mProjectionMode = mode; }

		/** Get the near distance.
		*/
		float getNearDistance() const { return mNearDistance; }

		/** Get the far distance.
		*/
		float getFarDistance() const { return mFarDistance; }

		/** Get the field of view in degrees.
		*/
		Degree getFieldOfView() const { return mFieldOfView; }

		/** Get the projection mode.
		*/
		ProjectionMode getProjectionMode() const { return mProjectionMode; }

		/** Get the view space transform.
		*/
		const Mat4& getTransform() const { return mTransform; }
	};
}
#endif // __Verdi_Camera_H__