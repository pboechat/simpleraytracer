#ifndef MATRIX3x3F_H_
#define MATRIX3x3F_H_

#include "Vector3F.h"

struct Matrix3x3F
{
	Matrix3x3F()
	{
		mMatrix[0] = mMatrix[4] = mMatrix[8] = 1.0f;
		mMatrix[1] = mMatrix[2] = mMatrix[3] = mMatrix[5] = mMatrix[6] = mMatrix[7] = 0.0f;
	}

	Matrix3x3F(float diagonal)
	{
		mMatrix[0] = mMatrix[4] = mMatrix[8] = diagonal;
		mMatrix[1] = mMatrix[2] = mMatrix[3] = mMatrix[5] = mMatrix[6] = mMatrix[7] = 0.0f;
	}

	Matrix3x3F(const Vector3F& column1, const Vector3F& column2, const Vector3F& column3)
	{
		mMatrix[0] = column1.x;
		mMatrix[1] = column2.x;
		mMatrix[2] = column3.x;
		mMatrix[3] = column1.y;
		mMatrix[4] = column2.y;
		mMatrix[5] = column3.y;
		mMatrix[6] = column1.z;
		mMatrix[7] = column2.z;
		mMatrix[8] = column3.z;
	}

	Matrix3x3F(float m11, float m12, float m13,
			   float m21, float m22, float m23,
			   float m31, float m32, float m33)
	{
		mMatrix[0] = m11; mMatrix[1] = m12;	mMatrix[2] = m13;
		mMatrix[3] = m21; mMatrix[4] = m22;	mMatrix[5] = m23;
		mMatrix[6] = m31; mMatrix[7] = m32; mMatrix[8] = m33;
	}

	inline Vector3F operator[] (unsigned int i) const
	{
		return Vector3F(mMatrix[i], mMatrix[i + 1], mMatrix[i + 2]);
	}

	inline Vector3F operator * (const Vector3F& rOther) const
	{
		float x = mMatrix[0] * rOther.x + mMatrix[1] * rOther.y + mMatrix[2] * rOther.z;
		float y = mMatrix[3] * rOther.x + mMatrix[4] * rOther.y + mMatrix[5] * rOther.z;
		float z = mMatrix[6] * rOther.x + mMatrix[7] * rOther.y + mMatrix[8] * rOther.z;
		return Vector3F(x, y, z);
	}

	inline Matrix3x3F operator * (const Matrix3x3F& rOther) const
	{
		/*
		* |a11b11 + a12b21 + a13b31            a11b12 + a12b22 + a13b32                a11b13 + a12b23 + a13b33|
		* |a21b11 + a22b21 + a23b31            a21b12 + a22b22 + a23b32                a21b13 + a22b23 + a23b33|
		* |a31b11 + a32b21 + a33b31            a31b12 + a32b22 + a33b32                a31b13 + a32b23 + a33b33|
		*/
		float m11 = mMatrix[0] * rOther.mMatrix[0] + mMatrix[1] * rOther.mMatrix[3] + mMatrix[2] * rOther.mMatrix[6];
		float m12 = mMatrix[0] * rOther.mMatrix[1] + mMatrix[1] * rOther.mMatrix[4] + mMatrix[2] * rOther.mMatrix[7];
		float m13 = mMatrix[0] * rOther.mMatrix[2] + mMatrix[1] * rOther.mMatrix[5] + mMatrix[2] * rOther.mMatrix[8];
		float m21 = mMatrix[3] * rOther.mMatrix[0] + mMatrix[4] * rOther.mMatrix[3] + mMatrix[5] * rOther.mMatrix[6];
		float m22 = mMatrix[3] * rOther.mMatrix[1] + mMatrix[4] * rOther.mMatrix[4] + mMatrix[5] * rOther.mMatrix[7];
		float m23 = mMatrix[3] * rOther.mMatrix[2] + mMatrix[4] * rOther.mMatrix[5] + mMatrix[5] * rOther.mMatrix[8];
		float m31 = mMatrix[6] * rOther.mMatrix[0] + mMatrix[7] * rOther.mMatrix[3] + mMatrix[8] * rOther.mMatrix[6];
		float m32 = mMatrix[6] * rOther.mMatrix[1] + mMatrix[7] * rOther.mMatrix[4] + mMatrix[8] * rOther.mMatrix[7];
		float m33 = mMatrix[6] * rOther.mMatrix[2] + mMatrix[7] * rOther.mMatrix[5] + mMatrix[8] * rOther.mMatrix[8];
		return Matrix3x3F(m11, m12, m13,
						  m21, m22, m23,
						  m31, m32, m33);
	}

	inline Matrix3x3F operator + (const Matrix3x3F& rOther) const
	{
		return Matrix3x3F(mMatrix[0] + rOther.mMatrix[0], mMatrix[1] + rOther.mMatrix[1], mMatrix[2] + rOther.mMatrix[2],
						  mMatrix[3] + rOther.mMatrix[3], mMatrix[4] + rOther.mMatrix[4], mMatrix[5] + rOther.mMatrix[5],
						  mMatrix[6] + rOther.mMatrix[6], mMatrix[7] + rOther.mMatrix[7], mMatrix[8] + rOther.mMatrix[8]);
	}

	inline Matrix3x3F& operator += (const Matrix3x3F& rOther)
	{
		mMatrix[0] += rOther.mMatrix[0]; mMatrix[1] += rOther.mMatrix[1]; mMatrix[2] += rOther.mMatrix[2];
		mMatrix[3] += rOther.mMatrix[3]; mMatrix[4] += rOther.mMatrix[4]; mMatrix[5] += rOther.mMatrix[5];
		mMatrix[6] += rOther.mMatrix[6]; mMatrix[7] += rOther.mMatrix[7]; mMatrix[8] += rOther.mMatrix[8];

		return *this;
	}

	inline Matrix3x3F operator - (const Matrix3x3F& rOther) const
	{
		return Matrix3x3F(mMatrix[0] - rOther.mMatrix[0], mMatrix[1] - rOther.mMatrix[1], mMatrix[2] - rOther.mMatrix[2],
						  mMatrix[3] - rOther.mMatrix[3], mMatrix[4] - rOther.mMatrix[4], mMatrix[5] - rOther.mMatrix[5],
						  mMatrix[6] - rOther.mMatrix[6], mMatrix[7] - rOther.mMatrix[7], mMatrix[8] - rOther.mMatrix[8]);
	}

	inline Matrix3x3F& operator -= (const Matrix3x3F& rOther)
	{
		mMatrix[0] -= rOther.mMatrix[0]; mMatrix[1] -= rOther.mMatrix[1]; mMatrix[2] -= rOther.mMatrix[2];
		mMatrix[3] -= rOther.mMatrix[3]; mMatrix[4] -= rOther.mMatrix[4]; mMatrix[5] -= rOther.mMatrix[5];
		mMatrix[6] -= rOther.mMatrix[6]; mMatrix[7] -= rOther.mMatrix[7]; mMatrix[8] -= rOther.mMatrix[8];

		return *this;
	}

	inline Matrix3x3F& operator = (const Matrix3x3F& rOther)
	{
		mMatrix[0] = rOther.mMatrix[0]; mMatrix[1] = rOther.mMatrix[1]; mMatrix[2] = rOther.mMatrix[2];
		mMatrix[3] = rOther.mMatrix[3]; mMatrix[4] = rOther.mMatrix[4]; mMatrix[5] = rOther.mMatrix[5];
		mMatrix[6] = rOther.mMatrix[6]; mMatrix[7] = rOther.mMatrix[7]; mMatrix[8] = rOther.mMatrix[8];

		return *this;
	}

	inline Matrix3x3F Transpose() const
	{
		return Matrix3x3F(mMatrix[0], mMatrix[3], mMatrix[6],
						  mMatrix[1], mMatrix[4], mMatrix[7],
						  mMatrix[2], mMatrix[5], mMatrix[8]);
	}

	inline float Determinant() const
	{
		return mMatrix[0] * (mMatrix[4] * mMatrix[8] - mMatrix[5] * mMatrix[7]) - 
			   mMatrix[1] * (mMatrix[8] * mMatrix[3] - mMatrix[5] * mMatrix[6]) + 
			   mMatrix[2] * (mMatrix[3] * mMatrix[7] - mMatrix[4] * mMatrix[6]);
	}

	inline Matrix3x3F Adjoint() const
	{
		Matrix3x3F& rTranspose = Transpose();

		float m11 = rTranspose.mMatrix[4] * rTranspose.mMatrix[8] - rTranspose.mMatrix[5] * rTranspose.mMatrix[7];
		float m12 = rTranspose.mMatrix[3] * rTranspose.mMatrix[8] - rTranspose.mMatrix[5] * rTranspose.mMatrix[6];
		float m13 = rTranspose.mMatrix[3] * rTranspose.mMatrix[7] - rTranspose.mMatrix[4] * rTranspose.mMatrix[6];
		float m21 = rTranspose.mMatrix[1] * rTranspose.mMatrix[8] - rTranspose.mMatrix[2] * rTranspose.mMatrix[7];
		float m22 = rTranspose.mMatrix[0] * rTranspose.mMatrix[8] - rTranspose.mMatrix[2] * rTranspose.mMatrix[6];
		float m23 = rTranspose.mMatrix[0] * rTranspose.mMatrix[7] - rTranspose.mMatrix[1] * rTranspose.mMatrix[6];
		float m31 = rTranspose.mMatrix[1] * rTranspose.mMatrix[5] - rTranspose.mMatrix[2] * rTranspose.mMatrix[4];
		float m32 = rTranspose.mMatrix[0] * rTranspose.mMatrix[5] - rTranspose.mMatrix[2] * rTranspose.mMatrix[3];
		float m33 = rTranspose.mMatrix[0] * rTranspose.mMatrix[4] - rTranspose.mMatrix[1] * rTranspose.mMatrix[3];

		return Matrix3x3F(m11, -m12, m13,
						  -m21, m22, -m23,
						  m31, -m32, m33);
	}

	inline Matrix3x3F Inverse() const
	{
		float determinant = Determinant();

		if (determinant == 0)
		{
			return Matrix3x3F();
		}

		return Adjoint() * (1.0f / determinant);
	}

private:
	float mMatrix[9];

};

typedef Matrix3x3F Matrix3F;

#endif