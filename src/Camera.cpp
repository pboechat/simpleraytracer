#include <cmath>

#include "Camera.h"
#include "SimpleRayTracerApp.h"

// NOTE: needs to be included after all other includes!
#include "Common.h"

//////////////////////////////////////////////////////////////////////////
Camera::Camera(float fov, float _near, float _far) :
	mFov(fov),
	mNear(_near),
	mFar(_far)
{
	float fovTan = tan(radian(mFov / 2.0f));
	mAspectRatio = SimpleRayTracerApp::SCREEN_WIDTH / (float)SimpleRayTracerApp::SCREEN_HEIGHT;
	mProjectionPlaneHeight = 2.0f * mNear * fovTan;
	mProjectionPlaneWidth = mAspectRatio * mProjectionPlaneHeight;
	float fovCot = 1.0f / fovTan;
	mProjection = Matrix4F(fovCot / mAspectRatio, 0, 0, 0,
						   0, fovCot, 0, 0,
						   /* left-handed system */
						   //0, 0,  mFar / (mFar - mNear), (-mNear * mFar) / (mFar - mNear), 
						   //0, 0, 1, 0);
						   /* right-handed */
						   0, 0,  mFar / (mNear - mFar), (mNear * mFar) / (mNear - mFar), 
						   0, 0, -1, 0);
}

//////////////////////////////////////////////////////////////////////////
void Camera::OnUpdate()
{
	mX = mWorldTransform.right();
	mY = mWorldTransform.up();
	mZ = mWorldTransform.forward();

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
	return Ray(mWorldTransform.position, (mNear * -mZ /* NOTE: handiness sensitive */) + (mProjectionPlaneHeight * (y / (float)SimpleRayTracerApp::SCREEN_HEIGHT - 0.5f) * mY) + (mProjectionPlaneWidth * (x / (float)SimpleRayTracerApp::SCREEN_WIDTH - 0.5f) * mX));
}