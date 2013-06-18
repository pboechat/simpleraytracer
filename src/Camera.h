#ifndef CAMERA_H_
#define CAMERA_H_

#include "Vector3F.h"
#include "Ray.h"

#include <cmath>

class Camera
{
public:
	Camera(unsigned int width, 
		   unsigned int height,
		   float FoV, 
		   float near, 
		   float far,
		   const Vector3F& rEyePosition, 
		   const Vector3F& rForward, 
		   const Vector3F rUp) :
	  mWidth(width),
	  mHeight(height),
	  mFoV(FoV),
	  mNear(near),
	  mFar(far),
	  mEyePosition(rEyePosition),
	  mForward(rForward),
	  mUp(rUp)
	{
		mAspectRatio = mWidth / (float)mHeight;
		mProjectionPlaneHeight = (float)(2.0 * mNear * tan(mFoV / 2.0));
		mProjectionPlaneWidth = mAspectRatio * mProjectionPlaneHeight;

		mZ = (rEyePosition - rForward).Normalized();
		mX = rUp.Cross(mZ);
		mY = mZ.Cross(mX);
	}

	~Camera()
	{
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