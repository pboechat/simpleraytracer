#ifndef MATRIX3x3F_H_
#define MATRIX3x3F_H_

#include "Vector3F.h"
#include "Matrix4x4F.h"

#define radian(x) x * 0.017453293f

struct Matrix3x3F
{
	//////////////////////////////////////////////////////////////////////////
	Matrix3x3F()
	{
		mMatrix[0] = mMatrix[4] = mMatrix[8] = 1.0f;
		mMatrix[1] = mMatrix[2] = mMatrix[3] = mMatrix[5] = mMatrix[6] = mMatrix[7] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Matrix3x3F(float diagonal)
	{
		mMatrix[0] = mMatrix[4] = mMatrix[8] = diagonal;
		mMatrix[1] = mMatrix[2] = mMatrix[3] = mMatrix[5] = mMatrix[6] = mMatrix[7] = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	Matrix3x3F(const Vector3F& column1, const Vector3F& column2, const Vector3F& column3)
	{
		mMatrix[0] = column1.x(); mMatrix[3] = column1.y(); mMatrix[6] = column1.z();
		mMatrix[1] = column2.x(); mMatrix[4] = column2.y(); mMatrix[7] = column2.z();
		mMatrix[2] = column3.x();	mMatrix[5] = column3.y();	mMatrix[8] = column3.z();
	}

	//////////////////////////////////////////////////////////////////////////
	Matrix3x3F(float m11, float m12, float m13,
			   float m21, float m22, float m23,
			   float mMatrix1, float mMatrix2, float mMatrix3)
	{
		mMatrix[0] = m11; mMatrix[1] = m12;	mMatrix[2] = m13;
		mMatrix[3] = m21; mMatrix[4] = m22;	mMatrix[5] = m23;
		mMatrix[6] = mMatrix1; mMatrix[7] = mMatrix2; mMatrix[8] = mMatrix3;
	}

	//////////////////////////////////////////////////////////////////////////
	inline float* operator[] (unsigned int i)
	{
		return &mMatrix[i * 3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline const float* operator[] (unsigned int i) const
	{
		return &mMatrix[i * 3];
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F GetColumn(unsigned int i) const
	{
		return Vector3F(mMatrix[0 + i], mMatrix[3 + i], mMatrix[6 + i]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Vector3F operator * (const Vector3F& rVector) const
	{
		float x = mMatrix[0] * rVector.x() + mMatrix[1] * rVector.y() + mMatrix[2] * rVector.z();
		float y = mMatrix[3] * rVector.x() + mMatrix[4] * rVector.y() + mMatrix[5] * rVector.z();
		float z = mMatrix[6] * rVector.x() + mMatrix[7] * rVector.y() + mMatrix[8] * rVector.z();
		return Vector3F(x, y, z);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F operator * (const Matrix3x3F& rOther) const
	{
		float m11 = mMatrix[0] * rOther.mMatrix[0] + mMatrix[1] * rOther.mMatrix[3] + mMatrix[2] * rOther.mMatrix[6];
		float m12 = mMatrix[0] * rOther.mMatrix[1] + mMatrix[1] * rOther.mMatrix[4] + mMatrix[2] * rOther.mMatrix[7];
		float m13 = mMatrix[0] * rOther.mMatrix[2] + mMatrix[1] * rOther.mMatrix[5] + mMatrix[2] * rOther.mMatrix[8];
		float m21 = mMatrix[3] * rOther.mMatrix[0] + mMatrix[4] * rOther.mMatrix[3] + mMatrix[5] * rOther.mMatrix[6];
		float m22 = mMatrix[3] * rOther.mMatrix[1] + mMatrix[4] * rOther.mMatrix[4] + mMatrix[5] * rOther.mMatrix[7];
		float m23 = mMatrix[3] * rOther.mMatrix[2] + mMatrix[4] * rOther.mMatrix[5] + mMatrix[5] * rOther.mMatrix[8];
		float mMatrix1 = mMatrix[6] * rOther.mMatrix[0] + mMatrix[7] * rOther.mMatrix[3] + mMatrix[8] * rOther.mMatrix[6];
		float mMatrix2 = mMatrix[6] * rOther.mMatrix[1] + mMatrix[7] * rOther.mMatrix[4] + mMatrix[8] * rOther.mMatrix[7];
		float mMatrix3 = mMatrix[6] * rOther.mMatrix[2] + mMatrix[7] * rOther.mMatrix[5] + mMatrix[8] * rOther.mMatrix[8];
		return Matrix3x3F(m11, m12, m13,
						  m21, m22, m23,
						  mMatrix1, mMatrix2, mMatrix3);
	}

	inline Matrix3x3F& operator *= (const Matrix3x3F& rOther)
	{
		float m11 = mMatrix[0] * rOther.mMatrix[0] + mMatrix[1] * rOther.mMatrix[3] + mMatrix[2] * rOther.mMatrix[6];
		float m12 = mMatrix[0] * rOther.mMatrix[1] + mMatrix[1] * rOther.mMatrix[4] + mMatrix[2] * rOther.mMatrix[7];
		float m13 = mMatrix[0] * rOther.mMatrix[2] + mMatrix[1] * rOther.mMatrix[5] + mMatrix[2] * rOther.mMatrix[8];
		float m21 = mMatrix[3] * rOther.mMatrix[0] + mMatrix[4] * rOther.mMatrix[3] + mMatrix[5] * rOther.mMatrix[6];
		float m22 = mMatrix[3] * rOther.mMatrix[1] + mMatrix[4] * rOther.mMatrix[4] + mMatrix[5] * rOther.mMatrix[7];
		float m23 = mMatrix[3] * rOther.mMatrix[2] + mMatrix[4] * rOther.mMatrix[5] + mMatrix[5] * rOther.mMatrix[8];
		float mMatrix1 = mMatrix[6] * rOther.mMatrix[0] + mMatrix[7] * rOther.mMatrix[3] + mMatrix[8] * rOther.mMatrix[6];
		float mMatrix2 = mMatrix[6] * rOther.mMatrix[1] + mMatrix[7] * rOther.mMatrix[4] + mMatrix[8] * rOther.mMatrix[7];
		float mMatrix3 = mMatrix[6] * rOther.mMatrix[2] + mMatrix[7] * rOther.mMatrix[5] + mMatrix[8] * rOther.mMatrix[8];

		mMatrix[0] = m11; mMatrix[1] = m12;	mMatrix[2] = m13;
		mMatrix[3] = m21; mMatrix[4] = m22;	mMatrix[5] = m23;
		mMatrix[6] = mMatrix1; mMatrix[7] = mMatrix2; mMatrix[8] = mMatrix3;

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F operator + (const Matrix3x3F& rOther) const
	{
		return Matrix3x3F(mMatrix[0] + rOther.mMatrix[0], mMatrix[1] + rOther.mMatrix[1], mMatrix[2] + rOther.mMatrix[2],
						  mMatrix[3] + rOther.mMatrix[3], mMatrix[4] + rOther.mMatrix[4], mMatrix[5] + rOther.mMatrix[5],
						  mMatrix[6] + rOther.mMatrix[6], mMatrix[7] + rOther.mMatrix[7], mMatrix[8] + rOther.mMatrix[8]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F& operator += (const Matrix3x3F& rOther)
	{
		mMatrix[0] += rOther.mMatrix[0]; mMatrix[1] += rOther.mMatrix[1]; mMatrix[2] += rOther.mMatrix[2];
		mMatrix[3] += rOther.mMatrix[3]; mMatrix[4] += rOther.mMatrix[4]; mMatrix[5] += rOther.mMatrix[5];
		mMatrix[6] += rOther.mMatrix[6]; mMatrix[7] += rOther.mMatrix[7]; mMatrix[8] += rOther.mMatrix[8];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F operator - (const Matrix3x3F& rOther) const
	{
		return Matrix3x3F(mMatrix[0] - rOther.mMatrix[0], mMatrix[1] - rOther.mMatrix[1], mMatrix[2] - rOther.mMatrix[2],
						  mMatrix[3] - rOther.mMatrix[3], mMatrix[4] - rOther.mMatrix[4], mMatrix[5] - rOther.mMatrix[5],
						  mMatrix[6] - rOther.mMatrix[6], mMatrix[7] - rOther.mMatrix[7], mMatrix[8] - rOther.mMatrix[8]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F& operator -= (const Matrix3x3F& rOther)
	{
		mMatrix[0] -= rOther.mMatrix[0]; mMatrix[1] -= rOther.mMatrix[1]; mMatrix[2] -= rOther.mMatrix[2];
		mMatrix[3] -= rOther.mMatrix[3]; mMatrix[4] -= rOther.mMatrix[4]; mMatrix[5] -= rOther.mMatrix[5];
		mMatrix[6] -= rOther.mMatrix[6]; mMatrix[7] -= rOther.mMatrix[7]; mMatrix[8] -= rOther.mMatrix[8];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F& operator = (const Matrix3x3F& rOther)
	{
		mMatrix[0] = rOther.mMatrix[0]; mMatrix[1] = rOther.mMatrix[1]; mMatrix[2] = rOther.mMatrix[2];
		mMatrix[3] = rOther.mMatrix[3]; mMatrix[4] = rOther.mMatrix[4]; mMatrix[5] = rOther.mMatrix[5];
		mMatrix[6] = rOther.mMatrix[6]; mMatrix[7] = rOther.mMatrix[7]; mMatrix[8] = rOther.mMatrix[8];

		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F Transpose() const
	{
		return Matrix3x3F(mMatrix[0], mMatrix[3], mMatrix[6],
						  mMatrix[1], mMatrix[4], mMatrix[7],
						  mMatrix[2], mMatrix[5], mMatrix[8]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline float Determinant() const
	{
		return mMatrix[0] * (mMatrix[4] * mMatrix[8] - mMatrix[5] * mMatrix[7]) - 
			   mMatrix[1] * (mMatrix[8] * mMatrix[3] - mMatrix[5] * mMatrix[6]) + 
			   mMatrix[2] * (mMatrix[3] * mMatrix[7] - mMatrix[4] * mMatrix[6]);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F Adjoint() const
	{
		Matrix3x3F& rTranspose = Transpose();

		float m11 = rTranspose.mMatrix[4] * rTranspose.mMatrix[8] - rTranspose.mMatrix[5] * rTranspose.mMatrix[7];
		float m12 = rTranspose.mMatrix[3] * rTranspose.mMatrix[8] - rTranspose.mMatrix[5] * rTranspose.mMatrix[6];
		float m13 = rTranspose.mMatrix[3] * rTranspose.mMatrix[7] - rTranspose.mMatrix[4] * rTranspose.mMatrix[6];
		float m21 = rTranspose.mMatrix[1] * rTranspose.mMatrix[8] - rTranspose.mMatrix[2] * rTranspose.mMatrix[7];
		float m22 = rTranspose.mMatrix[0] * rTranspose.mMatrix[8] - rTranspose.mMatrix[2] * rTranspose.mMatrix[6];
		float m23 = rTranspose.mMatrix[0] * rTranspose.mMatrix[7] - rTranspose.mMatrix[1] * rTranspose.mMatrix[6];
		float mMatrix1 = rTranspose.mMatrix[1] * rTranspose.mMatrix[5] - rTranspose.mMatrix[2] * rTranspose.mMatrix[4];
		float mMatrix2 = rTranspose.mMatrix[0] * rTranspose.mMatrix[5] - rTranspose.mMatrix[2] * rTranspose.mMatrix[3];
		float mMatrix3 = rTranspose.mMatrix[0] * rTranspose.mMatrix[4] - rTranspose.mMatrix[1] * rTranspose.mMatrix[3];

		return Matrix3x3F(m11, -m12, m13,
						  -m21, m22, -m23,
						  mMatrix1, -mMatrix2, mMatrix3);
	}

	//////////////////////////////////////////////////////////////////////////
	inline Matrix3x3F Inverse() const
	{
		float determinant = Determinant();

		if (determinant == 0)
		{
			return Matrix3x3F();
		}

		return Adjoint() * (1.0f / determinant);
	}

	//////////////////////////////////////////////////////////////////////////
	static Matrix3x3F AngleAxis(float angle, const Vector3F& rAxis)
	{
		float cosT = cos(radian(angle));
		float sinT = sin(radian(angle));
		float invCosT = 1 - cosT;

		float m11 = cosT + invCosT * (rAxis.x() * rAxis.x());
		float m12 = rAxis.y() * rAxis.x() * invCosT - rAxis.z() * sinT;
		float m13 = rAxis.z() * rAxis.x() * invCosT + rAxis.y() * sinT;
		float m21 = rAxis.x() * rAxis.y() * invCosT + rAxis.z() * sinT;
		float m22 = cosT + invCosT * (rAxis.y() * rAxis.y());
		float m23 = rAxis.z() * rAxis.y() * invCosT - rAxis.x() * sinT;
		float mMatrix1 = rAxis.x() * rAxis.z() * invCosT - rAxis.y() * sinT;
		float mMatrix2 = rAxis.y() * rAxis.z() * invCosT - rAxis.x() * sinT;
		float mMatrix3 = cosT + invCosT * (rAxis.z() * rAxis.z());

		return Matrix3x3F(m11, m12, m13,
						  m21, m22, m23,
						  mMatrix1, mMatrix2, mMatrix3);
	}

	inline Matrix4F ToMatrix4F() const
	{
		return Matrix4F(mMatrix[0], mMatrix[1], mMatrix[2], 0,
						mMatrix[3], mMatrix[4], mMatrix[5], 0,
						mMatrix[6], mMatrix[7], mMatrix[8], 0,
						0,			0,			0,			1);
	}

private:
	float mMatrix[9];

};

typedef Matrix3x3F Matrix3F;

#endif