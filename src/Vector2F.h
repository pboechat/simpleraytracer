#ifndef VECTOR2F_H_
#define VECTOR2F_H_

struct Vector2F
{
	float x;
	float y;

	//////////////////////////////////////////////////////////////////////////
	Vector2F() :
		x(0),
		y(0)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	Vector2F(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator + (const Vector2F& rOther) const
	{
		return Vector2F(x + rOther.x, y + rOther.y);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator += (const Vector2F& rOther)
	{
		x += rOther.x;
		y += rOther.y;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator - (const Vector2F& rOther) const
	{
		return Vector2F(x - rOther.x, y - rOther.y);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator -= (const Vector2F& rOther)
	{
		x -= rOther.x;
		y -= rOther.y;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator * (const Vector2F& rOther) const
	{
		return Vector2F(x * rOther.x, y * rOther.y);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator *= (const Vector2F& rOther)
	{
		x *= rOther.x;
		y *= rOther.y;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator + (float scalar) const
	{
		return Vector2F(x + scalar, y + scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator += (float scalar)
	{
		x += scalar;
		y += scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator - (float scalar) const
	{
		return Vector2F(x - scalar, y - scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator -= (float scalar)
	{
		x -= scalar;
		y -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator * (float scalar) const
	{
		return Vector2F(x * scalar, y * scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator *= (float scalar)
	{
		x *= scalar;
		y *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F operator / (float scalar) const
	{
		return Vector2F(x / scalar, y / scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator /= (float scalar)
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const Vector2F& rOther) const
	{
		return x == rOther.x && y == rOther.y;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const Vector2F& rOther) const
	{
		return x != rOther.x || y != rOther.y;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector2F& operator = (const Vector2F& rOther)
	{
		x = rOther.x;
		y = rOther.y;

		return *this;
	}

};

Vector2F operator + (float scalar, const Vector2F& rVector);
Vector2F operator - (float scalar, const Vector2F& rVector);
Vector2F operator * (float scalar, const Vector2F& rVector);
Vector2F operator / (float scalar, const Vector2F& rVector);
Vector2F operator - (const Vector2F& rVector);

#endif