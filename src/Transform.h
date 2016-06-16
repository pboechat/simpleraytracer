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

	inline Vector3F up()
	{
		return rotation.GetColumn(1);
	}

	inline Vector3F right()
	{
		return rotation.GetColumn(0);
	}

	inline Vector3F forward()
	{
		return rotation.GetColumn(2);
	}

	inline Matrix4F ToMatrix4x4F() const
	{
		Matrix4x4F model = rotation.ToMatrix4x4F();

		model[0][3] = position.x();
		model[1][3] = position.y();
		model[2][3] = position.z();

		return model * scale.ToMatrix4x4F();
	}

	void LookAt(const Vector3F& rForward, const Vector3F& rUp = Vector3F(0, 1, 0))
	{
		// NOTE: handiness sensitive
		Vector3F rZ = (position - rForward).Normalized();
		Vector3F rX = rUp.Cross(rZ).Normalized();
		Vector3F rY = rZ.Cross(rX);
		rotation = Matrix3F(rX, rY, rZ);
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