
#include "VerdiCamera.h"
#include "VerdiVec3.h"

namespace Verdi {

	Camera::Camera(float nearDist, float farDist, Degree fov)
		: mNearDistance(nearDist), mFarDistance(farDist), mFieldOfView(fov)
	{

	}

	void Camera::setIdentity()
	{
		mTransform.setIdentity();
	}

	void Camera::lookAt(Vec3 position, Vec3 target, Vec3 upVector)
	{
		// This function mimics the gluLookAt function.
		// The entire procedure is outlined in the gluLookAt
		// documentation (http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml)

		// The forward vector is defined as the target point - point
		Vec3 forwardVector = target - position;

		// Normalise the vectors
		forwardVector.normalise();
		upVector.normalise();

		// Take the cross product of the forward and up
		// to get the right
		Vec3 rightVector = forwardVector.cross(upVector);
		rightVector.normalise();

		// Take the cross product of the right and the forward
		// to get a new up vector.
		Vec3 finalUp = rightVector.cross(forwardVector);
		finalUp.normalise();

		// Now we make a new matrix and fill in the 
		// corresponding rows. 
		Mat4 mat;
		mat.setIdentity();

		mat._11 = rightVector.x; 
		mat._12 = rightVector.y;
		mat._13 = rightVector.z;

		mat._21 = finalUp.x; 
		mat._22 = finalUp.y;
		mat._23 = finalUp.z;

		mat._31 = -forwardVector.x;
		mat._32 = -forwardVector.y;
		mat._33 = -forwardVector.z;

		mat._41 = position.x;
		mat._42 = position.y;
		mat._43 = position.z;

		// Apply it to our current transformation
		mTransform = mat*mTransform;
	}
}