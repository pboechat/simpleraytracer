#ifndef COLORRGBA_H_
#define COLORRGBA_H_

#include "Common.h"

struct ColorRGBA
{
	static const ColorRGBA WHITE;

	//////////////////////////////////////////////////////////////////////////
	inline float& r()
	{
		return mValues[0];
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& g()
	{
		return mValues[1];
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& b()
	{
		return mValues[2];
	}

	//////////////////////////////////////////////////////////////////////////
	inline float& a()
	{
		return mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& r() const
	{
		return mValues[0];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& g() const
	{
		return mValues[1];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& b() const
	{
		return mValues[2];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float& a() const
	{
		return mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	ColorRGBA()
	{
		mValues[0] = mValues[1] = mValues[2] = mValues[3] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	ColorRGBA(float r, float g, float b, float a)
	{
		this->mValues[0] = r;
		this->mValues[1] = g;
		this->mValues[2] = b;
		this->mValues[3] = a;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator + (const ColorRGBA& rOther) const
	{
		return ColorRGBA(mValues[0] + rOther.mValues[0], mValues[1] + rOther.mValues[1], mValues[2] + rOther.mValues[2], mValues[3] + rOther.mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator += (const ColorRGBA& rOther)
	{
		mValues[0] += rOther.mValues[0];
		mValues[1] += rOther.mValues[1];
		mValues[2] += rOther.mValues[2];
		mValues[3] += rOther.mValues[3];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator - (const ColorRGBA& rOther) const
	{
		return ColorRGBA(mValues[0] - rOther.mValues[0], mValues[1] - rOther.mValues[1], mValues[2] - rOther.mValues[2], mValues[3] - rOther.mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator -= (const ColorRGBA& rOther)
	{
		mValues[0] -= rOther.mValues[0];
		mValues[1] -= rOther.mValues[1];
		mValues[2] -= rOther.mValues[2];
		mValues[3] -= rOther.mValues[3];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator * (const ColorRGBA& rOther) const
	{
		return ColorRGBA(mValues[0] * rOther.mValues[0], mValues[1] * rOther.mValues[1], mValues[2] * rOther.mValues[2], mValues[3] * rOther.mValues[3]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator *= (const ColorRGBA& rOther)
	{
		mValues[0] *= rOther.mValues[0];
		mValues[1] *= rOther.mValues[1];
		mValues[2] *= rOther.mValues[2];
		mValues[3] *= rOther.mValues[3];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator + (float scalar) const
	{
		return ColorRGBA(mValues[0] + scalar, mValues[1] + scalar, mValues[2] + scalar, mValues[3] + scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator += (float scalar)
	{
		mValues[0] += scalar;
		mValues[1] += scalar;
		mValues[2] += scalar;
		mValues[3] += scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator - (float scalar) const
	{
		return ColorRGBA(mValues[0] - scalar, mValues[1] - scalar, mValues[2] - scalar, mValues[3] - scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator -= (float scalar)
	{
		mValues[0] -= scalar;
		mValues[1] -= scalar;
		mValues[2] -= scalar;
		mValues[3] -= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator * (float scalar) const
	{
		return ColorRGBA(mValues[0] * scalar, mValues[1] * scalar, mValues[2] * scalar, mValues[3] * scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator *= (float scalar)
	{
		mValues[0] *= scalar;
		mValues[1] *= scalar;
		mValues[2] *= scalar;
		mValues[3] *= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA operator / (float scalar) const
	{
		return ColorRGBA(mValues[0] / scalar, mValues[1] / scalar, mValues[2] / scalar, mValues[3] / scalar);
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator /= (float scalar)
	{
		mValues[0] /= scalar;
		mValues[1] /= scalar;
		mValues[2] /= scalar;
		mValues[3] /= scalar;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator == (const ColorRGBA& rOther) const
	{
		return mValues[0] == rOther.mValues[0] && mValues[1] == rOther.mValues[1] && mValues[2] == rOther.mValues[2] && mValues[3] == rOther.mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline bool operator != (const ColorRGBA& rOther) const
	{
		return mValues[0] != rOther.mValues[0] || mValues[1] != rOther.mValues[1] || mValues[2] != rOther.mValues[2] || mValues[3] != rOther.mValues[3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA& operator = (const ColorRGBA& rOther)
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
	inline const float& operator[] (unsigned int i) const
	{
		return mValues[i];
	}

	//////////////////////////////////////////////////////////////////////////
	inline ColorRGBA Blend(const ColorRGBA& rDstColor) const
	{
		float a = srt_clamp(mValues[3], 0, 1);
		float invA = 1 - mValues[3];
		return ColorRGBA(a * mValues[0] + invA * rDstColor.mValues[0], 
						 a * mValues[1] + invA * rDstColor.mValues[1], 
						 a * mValues[2] + invA * rDstColor.mValues[2], 
						 a * mValues[3] + invA * rDstColor.mValues[3]);
	}

private:
	float mValues[4];

};

ColorRGBA operator + (float scalar, const ColorRGBA& rVector);
ColorRGBA operator - (float scalar, const ColorRGBA& rVector);
ColorRGBA operator * (float scalar, const ColorRGBA& rVector);
ColorRGBA operator / (float scalar, const ColorRGBA& rVector);
ColorRGBA operator - (const ColorRGBA& rVector);

#endif