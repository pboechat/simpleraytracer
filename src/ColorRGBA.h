#ifndef COLORRGBA_H_
#define COLORRGBA_H_

struct ColorRGBA
{
	float r;
	float g;
	float b;
	float a;

	//////////////////////////////////////////////////////////////////////////
	ColorRGBA() :
		r(0),
		g(0),
		b(0),
		a(1)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	ColorRGBA(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator + (const ColorRGBA& rOther) const
	{
		return ColorRGBA(r + rOther.r, g + rOther.g, b + rOther.b, a + rOther.a);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator += (const ColorRGBA& rOther)
	{
		r += rOther.r;
		g += rOther.g;
		b += rOther.b;
		a += rOther.a;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator - (const ColorRGBA& rOther) const
	{
		return ColorRGBA(r - rOther.r, g - rOther.g, b - rOther.b, a - rOther.a);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator -= (const ColorRGBA& rOther)
	{
		r -= rOther.r;
		g -= rOther.g;
		b -= rOther.b;
		a -= rOther.a;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator * (const ColorRGBA& rOther) const
	{
		return ColorRGBA(r * rOther.r, g * rOther.g, b * rOther.b, a * rOther.a);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator *= (const ColorRGBA& rOther)
	{
		r *= rOther.r;
		g *= rOther.g;
		b *= rOther.b;
		a *= rOther.a;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator + (float scalar) const
	{
		return ColorRGBA(r + scalar, g + scalar, b + scalar, a + scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator += (float scalar)
	{
		r += scalar;
		g += scalar;
		b += scalar;
		a += scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator - (float scalar) const
	{
		return ColorRGBA(r - scalar, g - scalar, b - scalar, a - scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator -= (float scalar)
	{
		r -= scalar;
		g -= scalar;
		b -= scalar;
		a -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator * (float scalar) const
	{
		return ColorRGBA(r * scalar, g * scalar, b * scalar, a * scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator *= (float scalar)
	{
		r *= scalar;
		g *= scalar;
		b *= scalar;
		a *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator / (float scalar) const
	{
		return ColorRGBA(r / scalar, g / scalar, b / scalar, a / scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator /= (float scalar)
	{
		r /= scalar;
		g /= scalar;
		b /= scalar;
		a /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const ColorRGBA& rOther) const
	{
		return r == rOther.r && g == rOther.g && b == rOther.b && a == rOther.a;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const ColorRGBA& rOther) const
	{
		return r != rOther.r || g != rOther.g || b != rOther.b || a != rOther.a;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator = (const ColorRGBA& rOther)
	{
		r = rOther.r;
		g = rOther.g;
		b = rOther.b;
		a = rOther.a;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA Blend(const ColorRGBA& rDstColor) const
	{
		float invA = 1 - a;
		return ColorRGBA(a * r + invA * rDstColor.r, a * g + invA * rDstColor.g, a * b + invA * rDstColor.b, a * a + invA * rDstColor.a);
	}

};

ColorRGBA operator + (float scalar, const ColorRGBA& rVector);
ColorRGBA operator - (float scalar, const ColorRGBA& rVector);
ColorRGBA operator * (float scalar, const ColorRGBA& rVector);
ColorRGBA operator / (float scalar, const ColorRGBA& rVector);
ColorRGBA operator - (const ColorRGBA& rVector);

#endif