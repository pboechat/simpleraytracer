#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "Vector3F.h"
#include "Matrix3x3F.h"
#include "Matrix4x4F.h"

struct Transform
{
	Matrix3F scale;
	Matrix3F rotation;
	Vector3F position;

	Transform()
	{
	}

	Transform(const Matrix3F& rScale, const Matrix3F& rRotation, const Vector3F& rPosition) :
		scale(rScale),
		rotation(rRotation),
		position(rPosition)
	{
	}

	~Transform()
	{
	}

	void LookAt(const Vector3F& rPosition, const Vector3F& rForward, const Vector3F& rUp)
	{
		Vector3F& rZ = (rPosition - rForward).Normalized();
		Vector3F& rX = rUp.Cross(rZ);
		Vector3F& rY = rZ.Cross(rX);

		rotation = Matrix3F(rX, rY, rZ);
		position = rPosition;
	}

	inline Vector3F operator * (const Vector3F& rVector) const
	{
		Vector3F vector = rVector;
		vector = scale * vector;
		vector = rotation * vector;
		vector += position;

		return vector;
	}

	inline Transform operator * (const Transform& rOther) const
	{
		return Transform(scale * rOther.scale, rotation * rOther.rotation, position + rOther.position);
	}

	inline Transform& operator *= (const Transform& rOther)
	{
		scale = rOther.scale * scale;
		rotation = rOther.rotation * rotation;
		position += rOther.position;

		return *this;
	}

	inline Transform& operator = (const Transform& rOther)
	{
		this->scale = rOther.scale;
		this->rotation = rOther.rotation;
		this->position = rOther.position;

		return *this;
	}

};

#endif