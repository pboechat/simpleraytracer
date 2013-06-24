#ifndef CAMERA_H_
#define CAMERA_H_

#include "Vector3F.h"
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

	inline void SetParameters(float fov, float nearZ, float farZ, const Vector3F& rEyePosition, const Vector3F& rForward, const Vector3F rUp)
	{
		mFoV = fov;
		mNear = nearZ;
		mFar = farZ;

		mEyePosition = rEyePosition;
		mForward = rForward;
		mUp = rUp;

		mZ = (rEyePosition - rForward).Normalized();
		mX = rUp.Cross(mZ);
		mY = mZ.Cross(mX);
	}

	inline void SetViewport(unsigned int width, unsigned int height)
	{
		mWidth = width;
		mHeight = height;
		mAspectRatio = mWidth / (float)mHeight;
		mProjectionPlaneHeight = (float)(2.0 * mNear * tan(DEG2RAD * mFoV / 2.0f));
		mProjectionPlaneWidth = mAspectRatio * mProjectionPlaneHeight;
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

	inline Matrix4x4F GetProjectionMatrix() const
	{
		// TODO:
		return Matrix4x4F();
	}

	inline Matrix4x4F GetViewMatrix() const
	{
		// TODO:
		return Matrix4x4F();
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
	float mFoV;
	float mNear;
	float mFar;
	unsigned int mWidth;
	unsigned int mHeight;
	float mAspectRatio;
	float mProjectionPlaneHeight;
	float mProjectionPlaneWidth;

};

#endif