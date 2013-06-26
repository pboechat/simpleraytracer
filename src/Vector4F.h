#ifndef VECTOR4F_H_
#define VECTOR4F_H_

#include <exception>

struct Vector4F
{
	float x;
	float y;
	float z;
	float w;

	//////////////////////////////////////////////////////////////////////////
	Vector4F() :
		x(0),
		y(0),
		z(0),
		w(0)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	Vector4F(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator + (const Vector4F& rOther) const
	{
		float scale = w / rOther.w;
		return Vector4F(x + (rOther.x * scale), y + (rOther.y * scale), z + (rOther.z * scale), w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator += (const Vector4F& rOther)
	{
		float scale = w / rOther.w;

		x += rOther.x * scale;
		y += rOther.y * scale;
		z += rOther.z * scale;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator - (const Vector4F& rOther) const
	{
		float scale = w / rOther.w;
		return Vector4F(x - (rOther.x * scale), y - (rOther.y * scale), z - (rOther.z * scale), w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator -= (const Vector4F& rOther)
	{
		float scale = w / rOther.w;

		x -= rOther.x * scale;
		y -= rOther.y * scale;
		z -= rOther.z * scale;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator * (const Vector4F& rOther) const
	{
		float scale = w / rOther.w;
		return Vector4F(x * rOther.x * scale, y * rOther.y * scale, z * rOther.z * scale, w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator *= (const Vector4F& rOther)
	{
		float scale = w / rOther.w;

		x *= rOther.x * scale;
		y *= rOther.y * scale;
		z *= rOther.z * scale;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator + (float scalar) const
	{
		scalar = scalar * w;
		return Vector4F(x + scalar, y + scalar, z + scalar, w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator += (float scalar)
	{
		scalar = scalar * w;

		x += scalar;
		y += scalar;
		z += scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator - (float scalar) const
	{
		scalar = scalar * w;
		return Vector4F(x - scalar, y - scalar, z - scalar, w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator -= (float scalar)
	{
		scalar = scalar * w;

		x -= scalar;
		y -= scalar;
		z -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator * (float scalar) const
	{
		scalar = scalar * w;
		return Vector4F(x * scalar, y * scalar, z * scalar, w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator *= (float scalar)
	{
		scalar = scalar * w;

		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator / (float scalar) const
	{
		scalar = scalar * w;
		return Vector4F(x / scalar, y / scalar, z / scalar, w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator /= (float scalar)
	{
		scalar = scalar * w;

		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const Vector4F& rOther) const
	{
		return x == rOther.x && y == rOther.y && z == rOther.z && w == rOther.w;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const Vector4F& rOther) const
	{
		return x != rOther.x || y != rOther.y || z != rOther.z || w != rOther.w;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F& operator = (const Vector4F& rOther)
	{
		x = rOther.x;
		y = rOther.y;
		z = rOther.z;
		w = rOther.w;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float operator[] (unsigned int i)
	{
		i = i % 3;
		if (i == 0)
		{
			return x;
		}
		else if (i == 1)
		{
			return y;
		}
		else if (i == 2)
		{
			return z;
		}
		else if (i == 3)
		{
			return w;
		}
		else
		{
			throw std::exception("invalid index");
		}
	}

};

Vector4F operator + (float scalar, const Vector4F& rVector);
Vector4F operator - (float scalar, const Vector4F& rVector);
Vector4F operator * (float scalar, const Vector4F& rVector);
Vector4F operator / (float scalar, const Vector4F& rVector);
Vector4F operator - (const Vector4F& rVector);

#endif