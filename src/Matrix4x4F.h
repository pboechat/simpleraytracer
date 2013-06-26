#ifndef MATRIX4X4F_H_
#define MATRIX4X4F_H_

#include "Vector4F.h"

class Matrix4x4F
{
public:
	//////////////////////////////////////////////////////////////////////////
	Matrix4x4F()
	{
		mMatrix[0] = mMatrix[5] = mMatrix[10] = mMatrix[15] = 1.0f;
		mMatrix[1] = mMatrix[2] = mMatrix[3] = mMatrix[4] = mMatrix[6] = mMatrix[7] = mMatrix[8] = mMatrix[9] = mMatrix[11] = mMatrix[12] = mMatrix[13] = mMatrix[14] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Matrix4x4F(float diagonal)
	{
		mMatrix[0] = mMatrix[5] = mMatrix[10] = mMatrix[15] = diagonal;
		mMatrix[1] = mMatrix[2] = mMatrix[3] = mMatrix[4] = mMatrix[6] = mMatrix[7] = mMatrix[8] = mMatrix[9] = mMatrix[11] = mMatrix[12] = mMatrix[13] = mMatrix[14] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Matrix4x4F(const Vector4F& column1, const Vector4F& column2, const Vector4F& column3, const Vector4F& column4)
	{
		mMatrix[0] = column1.x();	mMatrix[4] = column1.y();	mMatrix[8]  = column1.z();	 mMatrix[12] = column1.w();
		mMatrix[1] = column2.x();	mMatrix[5] = column2.y();	mMatrix[9]  = column2.z();	 mMatrix[13] = column2.w();
		mMatrix[2] = column3.x();	mMatrix[6] = column3.y();	mMatrix[10] = column3.z();	 mMatrix[14] = column3.w();
		mMatrix[3] = column4.x();	mMatrix[7] = column4.y();	mMatrix[11] = column4.z();	 mMatrix[15] = column4.w();
	}

	//////////////////////////////////////////////////////////////////////////
	Matrix4x4F(float m11, float m12, float m13, float m14,
			   float m21, float m22, float m23, float m24,
			   float m31, float m32, float m33, float m34,
			   float m41, float m42, float m43, float m44)
	{
		mMatrix[0]  = m11; mMatrix[1]  = m12; mMatrix[2]  = m13; mMatrix[3]  = m14;
		mMatrix[4]  = m21; mMatrix[5]  = m22; mMatrix[6]  = m23; mMatrix[7]  = m24;
		mMatrix[8]  = m31; mMatrix[9]  = m32; mMatrix[10] = m33; mMatrix[11] = m34;
		mMatrix[12] = m41; mMatrix[13] = m42; mMatrix[14] = m43; mMatrix[15] = m44;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float* operator[] (unsigned int i)
	{
		return &mMatrix[i * 4];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float* operator[] (unsigned int i) const
	{
		return &mMatrix[i * 4];
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector4F operator * (const Vector4F& rVector) const
	{
		float x = mMatrix[0] * rVector.x() + mMatrix[1] * rVector.y() + mMatrix[2] * rVector.z() + mMatrix[3] * rVector.w();
		float y = mMatrix[4] * rVector.x() + mMatrix[5] * rVector.y() + mMatrix[6] * rVector.z() + mMatrix[7] * rVector.w();
		float z = mMatrix[8] * rVector.x() + mMatrix[9] * rVector.y() + mMatrix[10] * rVector.z() + mMatrix[11] * rVector.w();
		float w = mMatrix[12] * rVector.x() + mMatrix[13] * rVector.y() + mMatrix[14] * rVector.z() + mMatrix[15] * rVector.w();
		return Vector4F(x, y, z, w);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F operator * (const Matrix4x4F& rOther) const
	{
		float m11 = mMatrix[0] * rOther.mMatrix[0] + mMatrix[1] * rOther.mMatrix[4] + mMatrix[2] * rOther.mMatrix[8] + mMatrix[3] * rOther.mMatrix[12];
		float m12 = mMatrix[0] * rOther.mMatrix[1] + mMatrix[1] * rOther.mMatrix[5] + mMatrix[2] * rOther.mMatrix[9] + mMatrix[3] * rOther.mMatrix[13];
		float m13 = mMatrix[0] * rOther.mMatrix[2] + mMatrix[1] * rOther.mMatrix[6] + mMatrix[2] * rOther.mMatrix[10] + mMatrix[3] * rOther.mMatrix[14];
		float m14 = mMatrix[0] * rOther.mMatrix[3] + mMatrix[1] * rOther.mMatrix[7] + mMatrix[2] * rOther.mMatrix[11] + mMatrix[3] * rOther.mMatrix[15];
		float m21 = mMatrix[4] * rOther.mMatrix[0] + mMatrix[5] * rOther.mMatrix[4] + mMatrix[6] * rOther.mMatrix[8] + mMatrix[7] * rOther.mMatrix[12];
		float m22 = mMatrix[4] * rOther.mMatrix[1] + mMatrix[5] * rOther.mMatrix[5] + mMatrix[6] * rOther.mMatrix[9] + mMatrix[7] * rOther.mMatrix[13];
		float m23 = mMatrix[4] * rOther.mMatrix[2] + mMatrix[5] * rOther.mMatrix[6] + mMatrix[6] * rOther.mMatrix[10] + mMatrix[7] * rOther.mMatrix[14];
		float m24 = mMatrix[4] * rOther.mMatrix[3] + mMatrix[5] * rOther.mMatrix[7] + mMatrix[6] * rOther.mMatrix[11] + mMatrix[7] * rOther.mMatrix[15];
		float m31 = mMatrix[8] * rOther.mMatrix[0] + mMatrix[9] * rOther.mMatrix[4] + mMatrix[10] * rOther.mMatrix[8] + mMatrix[11] * rOther.mMatrix[12];
		float m32 = mMatrix[8] * rOther.mMatrix[1] + mMatrix[9] * rOther.mMatrix[5] + mMatrix[10] * rOther.mMatrix[9] + mMatrix[11] * rOther.mMatrix[13];
		float m33 = mMatrix[8] * rOther.mMatrix[2] + mMatrix[9] * rOther.mMatrix[6] + mMatrix[10] * rOther.mMatrix[10] + mMatrix[11] * rOther.mMatrix[14];
		float m34 = mMatrix[8] * rOther.mMatrix[3] + mMatrix[9] * rOther.mMatrix[7] + mMatrix[10] * rOther.mMatrix[11] + mMatrix[11] * rOther.mMatrix[15];
		float m41 = mMatrix[12] * rOther.mMatrix[0] + mMatrix[13] * rOther.mMatrix[4] + mMatrix[14] * rOther.mMatrix[8] + mMatrix[15] * rOther.mMatrix[12];
		float m42 = mMatrix[12] * rOther.mMatrix[1] + mMatrix[13] * rOther.mMatrix[5] + mMatrix[14] * rOther.mMatrix[9] + mMatrix[15] * rOther.mMatrix[13];
		float m43 = mMatrix[12] * rOther.mMatrix[2] + mMatrix[13] * rOther.mMatrix[6] + mMatrix[14] * rOther.mMatrix[10] + mMatrix[15] * rOther.mMatrix[14];
		float m44 = mMatrix[12] * rOther.mMatrix[3] + mMatrix[13] * rOther.mMatrix[7] + mMatrix[14] * rOther.mMatrix[11] + mMatrix[15] * rOther.mMatrix[15];

		return Matrix4x4F(m11, m12, m13, m14,
						  m21, m22, m23, m24,
						  m31, m32, m33, m34,
						  m41, m42, m43, m44);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F& operator *= (const Matrix4x4F& rOther)
	{
		float m11 = mMatrix[0] * rOther.mMatrix[0] + mMatrix[1] * rOther.mMatrix[4] + mMatrix[2] * rOther.mMatrix[8] + mMatrix[3] * rOther.mMatrix[12];
		float m12 = mMatrix[0] * rOther.mMatrix[1] + mMatrix[1] * rOther.mMatrix[5] + mMatrix[2] * rOther.mMatrix[9] + mMatrix[3] * rOther.mMatrix[13];
		float m13 = mMatrix[0] * rOther.mMatrix[2] + mMatrix[1] * rOther.mMatrix[6] + mMatrix[2] * rOther.mMatrix[10] + mMatrix[3] * rOther.mMatrix[14];
		float m14 = mMatrix[0] * rOther.mMatrix[3] + mMatrix[1] * rOther.mMatrix[7] + mMatrix[2] * rOther.mMatrix[11] + mMatrix[3] * rOther.mMatrix[15];
		float m21 = mMatrix[4] * rOther.mMatrix[0] + mMatrix[5] * rOther.mMatrix[4] + mMatrix[6] * rOther.mMatrix[8] + mMatrix[7] * rOther.mMatrix[12];
		float m22 = mMatrix[4] * rOther.mMatrix[1] + mMatrix[5] * rOther.mMatrix[5] + mMatrix[6] * rOther.mMatrix[9] + mMatrix[7] * rOther.mMatrix[13];
		float m23 = mMatrix[4] * rOther.mMatrix[2] + mMatrix[5] * rOther.mMatrix[6] + mMatrix[6] * rOther.mMatrix[10] + mMatrix[7] * rOther.mMatrix[14];
		float m24 = mMatrix[4] * rOther.mMatrix[3] + mMatrix[5] * rOther.mMatrix[7] + mMatrix[6] * rOther.mMatrix[11] + mMatrix[7] * rOther.mMatrix[15];
		float m31 = mMatrix[8] * rOther.mMatrix[0] + mMatrix[9] * rOther.mMatrix[4] + mMatrix[10] * rOther.mMatrix[8] + mMatrix[11] * rOther.mMatrix[12];
		float m32 = mMatrix[8] * rOther.mMatrix[1] + mMatrix[9] * rOther.mMatrix[5] + mMatrix[10] * rOther.mMatrix[9] + mMatrix[11] * rOther.mMatrix[13];
		float m33 = mMatrix[8] * rOther.mMatrix[2] + mMatrix[9] * rOther.mMatrix[6] + mMatrix[10] * rOther.mMatrix[10] + mMatrix[11] * rOther.mMatrix[14];
		float m34 = mMatrix[8] * rOther.mMatrix[3] + mMatrix[9] * rOther.mMatrix[7] + mMatrix[10] * rOther.mMatrix[11] + mMatrix[11] * rOther.mMatrix[15];
		float m41 = mMatrix[12] * rOther.mMatrix[0] + mMatrix[13] * rOther.mMatrix[4] + mMatrix[14] * rOther.mMatrix[8] + mMatrix[15] * rOther.mMatrix[12];
		float m42 = mMatrix[12] * rOther.mMatrix[1] + mMatrix[13] * rOther.mMatrix[5] + mMatrix[14] * rOther.mMatrix[9] + mMatrix[15] * rOther.mMatrix[13];
		float m43 = mMatrix[12] * rOther.mMatrix[2] + mMatrix[13] * rOther.mMatrix[6] + mMatrix[14] * rOther.mMatrix[10] + mMatrix[15] * rOther.mMatrix[14];
		float m44 = mMatrix[12] * rOther.mMatrix[3] + mMatrix[13] * rOther.mMatrix[7] + mMatrix[14] * rOther.mMatrix[11] + mMatrix[15] * rOther.mMatrix[15];

		mMatrix[0] = m11; mMatrix[1] = m12;	mMatrix[2] = m13; mMatrix[3] = m14;
		mMatrix[4] = m21; mMatrix[5] = m22;	mMatrix[6] = m23; mMatrix[7] = m24;
		mMatrix[8] = m31; mMatrix[9] = m32; mMatrix[10] = m33; mMatrix[11] = m34;
		mMatrix[12] = m41; mMatrix[13] = m42; mMatrix[14] = m43; mMatrix[15] = m44;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F operator + (const Matrix4x4F& rOther) const
	{
		return Matrix4x4F(mMatrix[0]  + rOther.mMatrix[0], mMatrix[1]   + rOther.mMatrix[1], mMatrix[2]   + rOther.mMatrix[2], mMatrix[3]   + rOther.mMatrix[3], 
						  mMatrix[4]  + rOther.mMatrix[4], mMatrix[5]   + rOther.mMatrix[5], mMatrix[6]   + rOther.mMatrix[6], mMatrix[7]   + rOther.mMatrix[7], 
						  mMatrix[8]  + rOther.mMatrix[8], mMatrix[9]   + rOther.mMatrix[9], mMatrix[10]  + rOther.mMatrix[10], mMatrix[11] + rOther.mMatrix[11],
						  mMatrix[12] + rOther.mMatrix[12], mMatrix[13] + rOther.mMatrix[13], mMatrix[14] + rOther.mMatrix[14], mMatrix[15] + rOther.mMatrix[15]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F& operator += (const Matrix4x4F& rOther)
	{
		mMatrix[0]  += rOther.mMatrix[0]; mMatrix[1]   += rOther.mMatrix[1]; mMatrix[2]   += rOther.mMatrix[2]; mMatrix[3]   += rOther.mMatrix[3]; 
		mMatrix[4]  += rOther.mMatrix[4]; mMatrix[5]   += rOther.mMatrix[5]; mMatrix[6]   += rOther.mMatrix[6]; mMatrix[7]   += rOther.mMatrix[7]; 
		mMatrix[8]  += rOther.mMatrix[8]; mMatrix[9]   += rOther.mMatrix[9]; mMatrix[10]  += rOther.mMatrix[10]; mMatrix[11] += rOther.mMatrix[11];
		mMatrix[12] += rOther.mMatrix[12]; mMatrix[13] += rOther.mMatrix[13]; mMatrix[14] += rOther.mMatrix[14]; mMatrix[15] += rOther.mMatrix[15];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F operator - (const Matrix4x4F& rOther) const
	{
		return Matrix4x4F(mMatrix[0]  - rOther.mMatrix[0], mMatrix[1]   - rOther.mMatrix[1], mMatrix[2]   - rOther.mMatrix[2], mMatrix[3]   - rOther.mMatrix[3], 
						  mMatrix[4]  - rOther.mMatrix[4], mMatrix[5]   - rOther.mMatrix[5], mMatrix[6]   - rOther.mMatrix[6], mMatrix[7]   - rOther.mMatrix[7], 
						  mMatrix[8]  - rOther.mMatrix[8], mMatrix[9]   - rOther.mMatrix[9], mMatrix[10]  - rOther.mMatrix[10], mMatrix[11] - rOther.mMatrix[11],
						  mMatrix[12] - rOther.mMatrix[12], mMatrix[13] - rOther.mMatrix[13], mMatrix[14] - rOther.mMatrix[14], mMatrix[15] - rOther.mMatrix[15]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F& operator -= (const Matrix4x4F& rOther)
	{
		mMatrix[0]  -= rOther.mMatrix[0]; mMatrix[1]   -= rOther.mMatrix[1]; mMatrix[2]   -= rOther.mMatrix[2]; mMatrix[3]   -= rOther.mMatrix[3]; 
		mMatrix[4]  -= rOther.mMatrix[4]; mMatrix[5]   -= rOther.mMatrix[5]; mMatrix[6]   -= rOther.mMatrix[6]; mMatrix[7]   -= rOther.mMatrix[7]; 
		mMatrix[8]  -= rOther.mMatrix[8]; mMatrix[9]   -= rOther.mMatrix[9]; mMatrix[10]  -= rOther.mMatrix[10]; mMatrix[11] -= rOther.mMatrix[11];
		mMatrix[12] -= rOther.mMatrix[12]; mMatrix[13] -= rOther.mMatrix[13]; mMatrix[14] -= rOther.mMatrix[14]; mMatrix[15] -= rOther.mMatrix[15];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F& operator = (const Matrix4x4F& rOther)
	{
		mMatrix[0] =  rOther.mMatrix[0]; mMatrix[1]   = rOther.mMatrix[1]; mMatrix[2]   = rOther.mMatrix[2]; mMatrix[3]   = rOther.mMatrix[3]; 
		mMatrix[4] =  rOther.mMatrix[4]; mMatrix[5]   = rOther.mMatrix[5]; mMatrix[6]   = rOther.mMatrix[6]; mMatrix[7]   = rOther.mMatrix[7]; 
		mMatrix[8] =  rOther.mMatrix[8]; mMatrix[9]   = rOther.mMatrix[9]; mMatrix[10]  = rOther.mMatrix[10]; mMatrix[11] = rOther.mMatrix[11]; 
		mMatrix[12] = rOther.mMatrix[12]; mMatrix[13] = rOther.mMatrix[13]; mMatrix[14] = rOther.mMatrix[14]; mMatrix[15] = rOther.mMatrix[15]; 

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix4x4F Transpose() const
	{
		return Matrix4x4F(mMatrix[0], mMatrix[4], mMatrix[8], mMatrix[12],
						  mMatrix[1], mMatrix[5], mMatrix[9], mMatrix[13],
						  mMatrix[2], mMatrix[6], mMatrix[10], mMatrix[14],
						  mMatrix[3], mMatrix[7], mMatrix[11], mMatrix[15]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Determinant() const
	{
		return mMatrix[12] * mMatrix[9]  * mMatrix[6]  * mMatrix[3]   -  mMatrix[8] * mMatrix[13] * mMatrix[6]  * mMatrix[3]   -
			   mMatrix[12] * mMatrix[5]  * mMatrix[10] * mMatrix[3]   +  mMatrix[4] * mMatrix[13] * mMatrix[10] * mMatrix[3]   +
			   mMatrix[8]  * mMatrix[5]  * mMatrix[14] * mMatrix[3]   -  mMatrix[4] * mMatrix[9]  * mMatrix[14] * mMatrix[3]   -
			   mMatrix[12] * mMatrix[9]  * mMatrix[2]  * mMatrix[7]   +  mMatrix[8] * mMatrix[13] * mMatrix[2]  * mMatrix[7]   +
			   mMatrix[12] * mMatrix[1]  * mMatrix[10] * mMatrix[7]   -  mMatrix[0] * mMatrix[13] * mMatrix[10] * mMatrix[7]   -
			   mMatrix[8]  * mMatrix[1]  * mMatrix[14] * mMatrix[7]   +  mMatrix[0] * mMatrix[9]  * mMatrix[14] * mMatrix[7]   +
			   mMatrix[12] * mMatrix[5]  * mMatrix[2]  * mMatrix[11]  -  mMatrix[4] * mMatrix[13] * mMatrix[2]  * mMatrix[11]  -
			   mMatrix[12] * mMatrix[1]  * mMatrix[6]  * mMatrix[11]  +  mMatrix[0] * mMatrix[13] * mMatrix[6]  * mMatrix[11]  +
			   mMatrix[4]  * mMatrix[1]  * mMatrix[14] * mMatrix[11]  -  mMatrix[0] * mMatrix[5]  * mMatrix[14] * mMatrix[11]  -
			   mMatrix[8]  * mMatrix[5]  * mMatrix[2]  * mMatrix[15]  +  mMatrix[4] * mMatrix[9]  * mMatrix[2]  * mMatrix[15]  +
			   mMatrix[8]  * mMatrix[1]  * mMatrix[6]  * mMatrix[15]  -  mMatrix[0] * mMatrix[9]  * mMatrix[6]  * mMatrix[15]  -
			   mMatrix[4]  * mMatrix[1]  * mMatrix[10] * mMatrix[15]  +  mMatrix[0] * mMatrix[5]  * mMatrix[10] * mMatrix[15];
	}

private:
	float mMatrix[16];

};

typedef Matrix4x4F Matrix4F;

#endif