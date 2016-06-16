#ifndef VECTOR4F_H_
#define VECTOR4F_H_

#include "Vector3F.h"

struct Vector4F
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
	inline float& w()
	{
		return mValues[3];
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
	inline const float& w() const
	{
		return mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	Vector4F()
	{
		mValues[0] = mValues[1] = mValues[2] = mValues[3] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Vector4F(float x, float y, float z, float w)
	{
		mValues[0] = x;	mValues[1] = y;	mValues[2] = z;	mValues[3] = w;
	}

	//////////////////////////////////////////////////////////////////////////
	Vector4F(const Vector3F& v3f, float w)
	{
		mValues[0] = v3f.x();	mValues[1] = v3f.y();	mValues[2] = v3f.z();	mValues[3] = w;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator + (const Vector4F& rOther) const
	{
		float scale = mValues[3] / rOther.mValues[3];
		return Vector4F(mValues[0] + (rOther.mValues[0] * scale), mValues[1] + (rOther.mValues[1] * scale), mValues[2] + (rOther.mValues[2] * scale), mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator += (const Vector4F& rOther)
	{
		float scale = mValues[3] / rOther.mValues[3];

		mValues[0] += rOther.mValues[0] * scale;
		mValues[1] += rOther.mValues[1] * scale;
		mValues[2] += rOther.mValues[2] * scale;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator - (const Vector4F& rOther) const
	{
		float scale = mValues[3] / rOther.mValues[3];
		return Vector4F(mValues[0] - (rOther.mValues[0] * scale), mValues[1] - (rOther.mValues[1] * scale), mValues[2] - (rOther.mValues[2] * scale), mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator -= (const Vector4F& rOther)
	{
		float scale = mValues[3] / rOther.mValues[3];

		mValues[0] -= rOther.mValues[0] * scale;
		mValues[1] -= rOther.mValues[1] * scale;
		mValues[2] -= rOther.mValues[2] * scale;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator * (const Vector4F& rOther) const
	{
		float scale = mValues[3] / rOther.mValues[3];
		return Vector4F(mValues[0] * rOther.mValues[0] * scale, mValues[1] * rOther.mValues[1] * scale, mValues[2] * rOther.mValues[2] * scale, mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator *= (const Vector4F& rOther)
	{
		float scale = mValues[3] / rOther.mValues[3];

		mValues[0] *= rOther.mValues[0] * scale;
		mValues[1] *= rOther.mValues[1] * scale;
		mValues[2] *= rOther.mValues[2] * scale;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator + (float scalar) const
	{
		scalar = scalar * mValues[3];
		return Vector4F(mValues[0] + scalar, mValues[1] + scalar, mValues[2] + scalar, mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator += (float scalar)
	{
		scalar = scalar * mValues[3];

		mValues[0] += scalar;
		mValues[1] += scalar;
		mValues[2] += scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator - (float scalar) const
	{
		scalar = scalar * mValues[3];
		return Vector4F(mValues[0] - scalar, mValues[1] - scalar, mValues[2] - scalar, mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator -= (float scalar)
	{
		scalar = scalar * mValues[3];

		mValues[0] -= scalar;
		mValues[1] -= scalar;
		mValues[2] -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator * (float scalar) const
	{
		scalar = scalar * mValues[3];
		return Vector4F(mValues[0] * scalar, mValues[1] * scalar, mValues[2] * scalar, mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator *= (float scalar)
	{
		scalar = scalar * mValues[3];

		mValues[0] *= scalar;
		mValues[1] *= scalar;
		mValues[2] *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator / (float scalar) const
	{
		scalar = scalar * mValues[3];
		return Vector4F(mValues[0] / scalar, mValues[1] / scalar, mValues[2] / scalar, mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator /= (float scalar)
	{
		scalar = scalar * mValues[3];

		mValues[0] /= scalar;
		mValues[1] /= scalar;
		mValues[2] /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const Vector4F& rOther) const
	{
		return mValues[0] == rOther.mValues[0] && mValues[1] == rOther.mValues[1] && mValues[2] == rOther.mValues[2] && mValues[3] == rOther.mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const Vector4F& rOther) const
	{
		return mValues[0] != rOther.mValues[0] || mValues[1] != rOther.mValues[1] || mValues[2] != rOther.mValues[2] || mValues[3] != rOther.mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator = (const Vector4F& rOther)
	{
		mValues[0] = rOther.mValues[0];
		mValues[1] = rOther.mValues[1];
		mValues[2] = rOther.mValues[2];
		mValues[3] = rOther.mValues[3];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& operator[] (unsigned int i)
	{
		return mValues[i];
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F xyz() const
	{
		return Vector3F(mValues[0], mValues[1], mValues[2]);
	}

private:
	float mValues[4];

};

Vector4F operator + (float scalar, const Vector4F& rVector);
Vector4F operator - (float scalar, const Vector4F& rVector);
Vector4F operator * (float scalar, const Vector4F& rVector);
Vector4F operator / (float scalar, const Vector4F& rVector);
Vector4F operator - (const Vector4F& rVector);

#endif