#include "Camera.h"
#include "Application.h"

#include <cmath>

#include "Common.h"

//////////////////////////////////////////////////////////////////////////
Camera::Camera(float fov, float _zNear, float _zFar) :
	mFov(fov),
	mNear(_zNear),
	mFar(_zFar)
{
	mAspectRatio = Application::SCREEN_WIDTH / (float)Application::SCREEN_HEIGHT;
	mProjectionPlaneHeight = (float)(2.0 * mNear * tan(radian(mFov) / 2.0f));
	mProjectionPlaneWidth = mAspectRatio * mProjectionPlaneHeight;

	float f = 1.0f / tan(radian(mFov) / 2.0f);
	mProjection = Matrix4F(f / mAspectRatio, 0, 0, 0,
						   0, f, 0, 0,
						   0, 0, (mFar + mNear) / (mNear - mFar), (2 * mFar * mNear) / (mNear - mFar),
						   0, 0, -1, 0);
}

//////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
}

//////////////////////////////////////////////////////////////////////////
void Camera::OnUpdate()
{
	mX = mWorldTransform.rotation.GetColumn(0);
	mY = mWorldTransform.rotation.GetColumn(1);
	mZ = mWorldTransform.rotation.GetColumn(2);

	// the inverse of a "pure rotation" matrix is its transpose
	mInverseRotation = mWorldTransform.rotation.Transpose().ToMatrix4x4F();

	mView = mInverseRotation * Matrix4F(1, 0, 0, -mWorldTransform.position.x(), 
	 						            0, 1, 0, -mWorldTransform.position.y(), 
	 						            0, 0, 1, -mWorldTransform.position.z(), 
							            0, 0, 0, 1);
}

//////////////////////////////////////////////////////////////////////////
Ray Camera::GetRayFromScreenCoordinates(unsigned int x, unsigned int y) const
{
	Vector3F direction = (-mNear * mZ) + (mProjectionPlaneHeight * (y / (float)Application::SCREEN_HEIGHT - 0.5f) * mY) + (mProjectionPlaneWidth * (x / (float)Application::SCREEN_WIDTH - 0.5f) * mX);
	return Ray(mWorldTransform.position, direction);
}