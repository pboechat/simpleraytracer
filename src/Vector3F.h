#ifndef VECTOR3F_H_
#define VECTOR3F_H_

#include <cmath>

struct Vector3F
{
	float x;
	float y;
	float z;

	//////////////////////////////////////////////////////////////////////////
	Vector3F() :
		x(0),
		y(0),
		z(0)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	Vector3F(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator + (const Vector3F& rOther) const
	{
		return Vector3F(x + rOther.x, y + rOther.y, z + rOther.z);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator += (const Vector3F& rOther)
	{
		x += rOther.x;
		y += rOther.y;
		z += rOther.z;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator - (const Vector3F& rOther) const
	{
		return Vector3F(x - rOther.x, y - rOther.y, z - rOther.z);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator -= (const Vector3F& rOther)
	{
		x -= rOther.x;
		y -= rOther.y;
		z -= rOther.z;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator * (const Vector3F& rOther) const
	{
		return Vector3F(x * rOther.x, y * rOther.y, z * rOther.z);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator *= (const Vector3F& rOther)
	{
		x *= rOther.x;
		y *= rOther.y;
		z *= rOther.z;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator + (float scalar) const
	{
		return Vector3F(x + scalar, y + scalar, z + scalar);
	}
	
	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator += (float scalar)
	{
		x += scalar;
		y += scalar;
		z += scalar;

		return *this;
	}
	
	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator - (float scalar) const
	{
		return Vector3F(x - scalar, y - scalar, z - scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator -= (float scalar)
	{
		x -= scalar;
		y -= scalar;
		z -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator * (float scalar) const
	{
		return Vector3F(x * scalar, y * scalar, z * scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator *= (float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator / (float scalar) const
	{
		return Vector3F(x / scalar, y / scalar, z / scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator /= (float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const Vector3F& rOther) const
	{
		return x == rOther.x && y == rOther.y && z == rOther.z;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const Vector3F& rOther) const
	{
		return x != rOther.x || y != rOther.y || z != rOther.z;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F& operator = (const Vector3F& rOther)
	{
		x = rOther.x;
		y = rOther.y;
		z = rOther.z;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Distance(const Vector3F& rOther) const
	{
		float x1 = rOther.x - x;
		float y1 = rOther.y - y;
		float z1 = rOther.z - z;
		return sqrt((x1 * x1) + (y1 * y1) + (z1 * z1));
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Length() const
	{
		return sqrt(x * x + y * y + z * z);
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
		x /= length;
		y /= length;
		z /= length;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F Normalized()
	{
		float length = Length();
		return Vector3F(x / length, y / length, z / length);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Dot(const Vector3F& rOther) const
	{
		return x * rOther.x + y * rOther.y + z * rOther.z;
	}
	
	//////////////////////////////////////////////////////////////////////////
	inline Vector3F Cross(const Vector3F& rOther) const
	{
		return Vector3F(y * rOther.z - z * rOther.y,
						z * rOther.x - x * rOther.z,
						x * rOther.y - y * rOther.x);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Angle(const Vector3F& rOther) const
	{
		return acos(Dot(rOther) / (Length() * rOther.Length()));
	}

};

Vector3F operator + (float scalar, const Vector3F& rVector);
Vector3F operator - (float scalar, const Vector3F& rVector);
Vector3F operator * (float scalar, const Vector3F& rVector);
Vector3F operator / (float scalar, const Vector3F& rVector);
Vector3F operator - (const Vector3F& rVector);

#endif