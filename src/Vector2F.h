#ifndef VECTOR2F_H_
#define VECTOR2F_H_

struct Vector2F
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
	Vector2F()
	{
		mValues[0] = mValues[1] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Vector2F(float x, float y)
	{
		mValues[0] = x; mValues[1] = y;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator + (const Vector2F& rOther) const
	{
		return Vector2F(mValues[0] + rOther.mValues[0], mValues[1] + rOther.mValues[1]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator += (const Vector2F& rOther)
	{
		mValues[0] += rOther.mValues[0];
		mValues[1] += rOther.mValues[1];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator - (const Vector2F& rOther) const
	{
		return Vector2F(mValues[0] - rOther.mValues[0], mValues[1] - rOther.mValues[1]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator -= (const Vector2F& rOther)
	{
		mValues[0] -= rOther.mValues[0];
		mValues[1] -= rOther.mValues[1];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator * (const Vector2F& rOther) const
	{
		return Vector2F(mValues[0] * rOther.mValues[0], mValues[1] * rOther.mValues[1]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator *= (const Vector2F& rOther)
	{
		mValues[0] *= rOther.mValues[0];
		mValues[1] *= rOther.mValues[1];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator + (float scalar) const
	{
		return Vector2F(mValues[0] + scalar, mValues[1] + scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator += (float scalar)
	{
		mValues[0] += scalar;
		mValues[1] += scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator - (float scalar) const
	{
		return Vector2F(mValues[0] - scalar, mValues[1] - scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator -= (float scalar)
	{
		mValues[0] -= scalar;
		mValues[1] -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator * (float scalar) const
	{
		return Vector2F(mValues[0] * scalar, mValues[1] * scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator *= (float scalar)
	{
		mValues[0] *= scalar;
		mValues[1] *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator / (float scalar) const
	{
		return Vector2F(mValues[0] / scalar, mValues[1] / scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator /= (float scalar)
	{
		mValues[0] /= scalar;
		mValues[1] /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const Vector2F& rOther) const
	{
		return mValues[0] == rOther.mValues[0] && mValues[1] == rOther.mValues[1];
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const Vector2F& rOther) const
	{
		return mValues[0] != rOther.mValues[0] || mValues[1] != rOther.mValues[1];
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator = (const Vector2F& rOther)
	{
		mValues[0] = rOther.mValues[0];
		mValues[1] = rOther.mValues[1];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& operator[] (unsigned int i)
	{
		return mValues[i];
	}

private:
	float mValues[2];

};

Vector2F operator + (float scalar, const Vector2F& rVector);
Vector2F operator - (float scalar, const Vector2F& rVector);
Vector2F operator * (float scalar, const Vector2F& rVector);
Vector2F operator / (float scalar, const Vector2F& rVector);
Vector2F operator - (const Vector2F& rVector);

#endif