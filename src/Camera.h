#ifndef CAMERA_H_
#define CAMERA_H_

#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix4x4F.h"
#include "Ray.h"

#include <cmath>

#define DEG2RAD 0.017453293f

class Camera
{
public:
	Camera()
	{
	}

	~Camera()
	{
	}

	inline void SetParameters(float fov, float zNear, float zFar, const Vector3F& rEyePosition, const Vector3F& rForward, const Vector3F rUp)
	{
		mFov = fov;
		mNear = zNear;
		mFar = zFar;

		mEyePosition = rEyePosition;
		mForward = rForward;
		mUp = rUp;

		mZ = (rEyePosition - rForward).Normalized();
		mX = rUp.Cross(mZ);
		mY = mZ.Cross(mX);

		// the inverse of a pure rotation matrix is its transpose
		mInverseRotationMatrix = Matrix4F(mX.x(), mX.y(), mX.z(), 0,
										  mY.x(), mY.y(), mY.z(), 0,
										  mZ.x(), mZ.y(), mZ.z(), 0,
										  0,      0,      0,      1);

		// "inverse" translation
		Matrix4F translation(1, 0, 0, -mEyePosition.x(), 
							 0, 1, 0, -mEyePosition.y(), 
							 0, 0, 1, -mEyePosition.z(), 
							 0, 0, 0, 1);

		mViewMatrix = mInverseRotationMatrix * translation;
	}

	inline void SetViewport(unsigned int width, unsigned int height)
	{
		mWidth = width;
		mHeight = height;
		mAspectRatio = mWidth / (float)mHeight;
		mProjectionPlaneHeight = (float)(2.0 * mNear * tan(DEG2RAD * mFov / 2.0f));
		mProjectionPlaneWidth = mAspectRatio * mProjectionPlaneHeight;

		float f = 1.0f / tan(DEG2RAD * mFov / 2.0f);
		mProjectionMatrix = Matrix4F(f / mAspectRatio, 0, 0, 0,
									 0, f, 0, 0,
									 0, 0, (mFar + mNear) / (mNear - mFar), (2 * mFar * mNear) / (mNear - mFar),
									 0, 0, -1, 0);
	}

	inline unsigned int GetWidth() const
	{
		return mWidth;
	}

	inline unsigned int GetHeight() const
	{
		return mHeight;
	}

	inline float GetNear() const
	{
		return mNear;
	}

	inline float GetFar() const
	{
		return mFar;
	}

	inline const Vector3F& GetEyePosition() const
	{
		return mEyePosition;
	}

	inline const Matrix4F& GetProjectionMatrix() const
	{
		return mProjectionMatrix;
	}

	inline const Matrix4F& GetViewMatrix() const
	{
		return mViewMatrix;
	}

	inline const Matrix4F& GetInverseRotationMatrix() const
	{
		return mInverseRotationMatrix;
	}

	inline Ray GetRayFromScreenCoordinates(unsigned int x, unsigned int y) const
	{
		Vector3F direction = (-mNear * mZ) + (mProjectionPlaneHeight * ((float)y / (float)mHeight - 0.5f) * mY) + (mProjectionPlaneWidth * ((float)x / (float)mWidth - 0.5f) * mX);
		return Ray(mEyePosition, direction);
	}

private:
	Vector3F mEyePosition;
	Vector3F mForward;
	Vector3F mUp;
	Vector3F mX;
	Vector3F mY;
	Vector3F mZ;
	float mFov;
	float mNear;
	float mFar;
	Matrix4F mProjectionMatrix;
	Matrix4F mViewMatrix;
	Matrix4F mInverseRotationMatrix;
	unsigned int mWidth;
	unsigned int mHeight;
	float mAspectRatio;
	float mProjectionPlaneHeight;
	float mProjectionPlaneWidth;

};

#endif