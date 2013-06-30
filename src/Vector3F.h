#ifndef VECTOR3F_H_
#define VECTOR3F_H_

#include "Vector4F.h"

#include <cmath>

struct Vector3F
{
	//////////////////////////////////////////////////////////////////////////
	inline float& x()
	{
		return mValues[0];
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& y()
	{
		return mValues[1];
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& z()
	{
		return mValues[2];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& x() const
	{
		return mValues[0];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& y() const
	{
		return mValues[1];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& z() const
	{
		return mValues[2];
	}

	//////////////////////////////////////////////////////////////////////////
	Vector3F()
	{
		mValues[0] = mValues[1] = mValues[2] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Vector3F(float x, float y, float z)
	{
		mValues[0] = x; mValues[1] = y; mValues[2] = z;
	}

	//////////////////////////////////////////////////////////////////////////
	Vector3F(const Vector4F& rVector4F)
	{
		mValues[0] = rVector4F.x() / rVector4F.w();
		mValues[1] = rVector4F.y() / rVector4F.w();
		mValues[2] = rVector4F.z() / rVector4F.w();
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator + (const Vector3F& rOther) const
	{
		return Vector3F(mValues[0] + rOther.mValues[0], mValues[1] + rOther.mValues[1], mValues[2] + rOther.mValues[2]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator += (const Vector3F& rOther)
	{
		mValues[0] += rOther.mValues[0];
		mValues[1] += rOther.mValues[1];
		mValues[2] += rOther.mValues[2];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator - (const Vector3F& rOther) const
	{
		return Vector3F(mValues[0] - rOther.mValues[0], mValues[1] - rOther.mValues[1], mValues[2] - rOther.mValues[2]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator -= (const Vector3F& rOther)
	{
		mValues[0] -= rOther.mValues[0];
		mValues[1] -= rOther.mValues[1];
		mValues[2] -= rOther.mValues[2];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator * (const Vector3F& rOther) const
	{
		return Vector3F(mValues[0] * rOther.mValues[0], mValues[1] * rOther.mValues[1], mValues[2] * rOther.mValues[2]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator *= (const Vector3F& rOther)
	{
		mValues[0] *= rOther.mValues[0];
		mValues[1] *= rOther.mValues[1];
		mValues[2] *= rOther.mValues[2];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator + (float scalar) const
	{
		return Vector3F(mValues[0] + scalar, mValues[1] + scalar, mValues[2] + scalar);
	}
	
	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator += (float scalar)
	{
		mValues[0] += scalar;
		mValues[1] += scalar;
		mValues[2] += scalar;

		return *this;
	}
	
	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator - (float scalar) const
	{
		return Vector3F(mValues[0] - scalar, mValues[1] - scalar, mValues[2] - scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator -= (float scalar)
	{
		mValues[0] -= scalar;
		mValues[1] -= scalar;
		mValues[2] -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator * (float scalar) const
	{
		return Vector3F(mValues[0] * scalar, mValues[1] * scalar, mValues[2] * scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator *= (float scalar)
	{
		mValues[0] *= scalar;
		mValues[1] *= scalar;
		mValues[2] *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator / (float scalar) const
	{
		return Vector3F(mValues[0] / scalar, mValues[1] / scalar, mValues[2] / scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator /= (float scalar)
	{
		mValues[0] /= scalar;
		mValues[1] /= scalar;
		mValues[2] /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const Vector3F& rOther) const
	{
		return mValues[0] == rOther.mValues[0] && mValues[1] == rOther.mValues[1] && mValues[2] == rOther.mValues[2];
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const Vector3F& rOther) const
	{
		return mValues[0] != rOther.mValues[0] || mValues[1] != rOther.mValues[1] || mValues[2] != rOther.mValues[2];
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator = (const Vector3F& rOther)
	{
		mValues[0] = rOther.mValues[0];
		mValues[1] = rOther.mValues[1];
		mValues[2] = rOther.mValues[2];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& operator[] (unsigned int i)
	{
		return mValues[i];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& operator[] (unsigned int i) const
	{
		return mValues[i];
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Distance(const Vector3F& rOther) const
	{
		float x1 = rOther.mValues[0] - mValues[0];
		float y1 = rOther.mValues[1] - mValues[1];
		float z1 = rOther.mValues[2] - mValues[2];
		return sqrt(x1 * x1 + y1 * y1 + z1 * z1);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Length() const
	{
		return sqrt(mValues[0] * mValues[0] + mValues[1] * mValues[1] + mValues[2] * mValues[2]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F Reflection(const Vector3F& rOther)
	{
		return this->operator-(rOther * (2.0f * Dot(rOther) / pow(rOther.Length(), 2)));
	}

	//////////////////////////////////////////////////////////////////////////
	inline void Normalize()
	{
		float length = Length();
		mValues[0] /= length;
		mValues[1] /= length;
		mValues[2] /= length;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F Normalized()
	{
		float length = Length();
		return Vector3F(mValues[0] / length, mValues[1] / length, mValues[2] / length);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Dot(const Vector3F& rOther) const
	{
		return mValues[0] * rOther.mValues[0] + mValues[1] * rOther.mValues[1] + mValues[2] * rOther.mValues[2];
	}
	
	//////////////////////////////////////////////////////////////////////////
	inline Vector3F Cross(const Vector3F& rOther) const
	{
		return Vector3F(mValues[1] * rOther.mValues[2] - mValues[2] * rOther.mValues[1],
						mValues[2] * rOther.mValues[0] - mValues[0] * rOther.mValues[2],
						mValues[0] * rOther.mValues[1] - mValues[1] * rOther.mValues[0]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Angle(const Vector3F& rOther) const
	{
		return acos(Dot(rOther) / (Length() * rOther.Length()));
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F ToVector4F() const
	{
		return Vector4F(mValues[0], mValues[1], mValues[2], 1);
	}

private:
	float mValues[3];

};

Vector3F operator + (float scalar, const Vector3F& rVector);
Vector3F operator - (float scalar, const Vector3F& rVector);
Vector3F operator * (float scalar, const Vector3F& rVector);
Vector3F operator / (float scalar, const Vector3F& rVector);
Vector3F operator - (const Vector3F& rVector);

#endif