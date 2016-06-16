#ifndef CAMERA_H_
#define CAMERA_H_

#include "SceneObject.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix4x4F.h"
#include "Ray.h"

struct Camera : public SceneObject
{
public:
	Camera(float fov, float near, float far);
	virtual ~Camera() = default;

	inline const Matrix4F& projection() const
	{
		return mProjection;
	}

	inline const Matrix4F& view() const
	{
		return mView;
	}

	inline const Matrix4F& inverseRotation() const
	{
		return mInverseRotation;
	}
	
	inline float zNear() const
	{
		return mNear;
	}

	inline float zFar() const
	{
		return mFar;
	}

	Ray GetRayFromScreenCoordinates(unsigned int x, unsigned int y) const;

protected:
	virtual void OnUpdate();

private:
	Vector3F mEyePosition;
	Vector3F mX;
	Vector3F mY;
	Vector3F mZ;
	float mFov;
	float mNear;
	float mFar;
	Matrix4F mProjection;
	Matrix4F mView;
	Matrix4F mInverseRotation;
	float mAspectRatio;
	float mProjectionPlaneHeight;
	float mProjectionPlaneWidth;

};

#endif