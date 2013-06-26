#include "EigenSolver.h"

#include <math.h>
#include <cmath>

//////////////////////////////////////////////////////////////////////////
EigenSolver::EigenSolver(const Matrix3F& rMatrix)
{
	mIsRotation = false;

	mMatrix[0][0] = rMatrix[0][1]; mMatrix[0][1] = rMatrix[0][1]; mMatrix[0][2] = rMatrix[0][2];
	mMatrix[1][0] = rMatrix[1][0]; mMatrix[1][1] = rMatrix[1][1]; mMatrix[1][2] = rMatrix[1][2];
	mMatrix[2][0] = rMatrix[2][0]; mMatrix[2][1] = rMatrix[2][1]; mMatrix[2][2] = rMatrix[2][2];
}

//////////////////////////////////////////////////////////////////////////
void EigenSolver::Solve()
{
	Tridiagonal();
	QLAlgorithm();
	GuaranteeRotation();
}

//////////////////////////////////////////////////////////////////////////
void EigenSolver::SortDescreasingly()
{
	Tridiagonal();
	QLAlgorithm();
	DecreasingSort();
	GuaranteeRotation();
}

//////////////////////////////////////////////////////////////////////////
void EigenSolver::SortIncreasingly()
{
	Tridiagonal();
	QLAlgorithm();
	IncreasingSort();
	GuaranteeRotation();
}

//////////////////////////////////////////////////////////////////////////
float EigenSolver::GetEigenValue(int i) const
{
	return mDiagonal[i];
}

//////////////////////////////////////////////////////////////////////////
Vector3F EigenSolver::GetEigenVector(int i) const
{
	return Vector3F(mMatrix[0][i], mMatrix[1][i], mMatrix[2][i]);
}

//////////////////////////////////////////////////////////////////////////
void EigenSolver::GuaranteeRotation()
{
	if (!mIsRotation)
	{
		// change sign on the first column
		for (int iRow = 0; iRow < 3; iRow++)
		{
			mMatrix[iRow][0] = -mMatrix[iRow][0];
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void EigenSolver::Tridiagonal()
{
	float fM00 = mMatrix[0][0];
	float fM01 = mMatrix[0][1];
	float fM02 = mMatrix[0][2];
	float fM11 = mMatrix[1][1];
	float fM12 = mMatrix[1][2];
	float fM22 = mMatrix[2][2];
	mDiagonal[0] = fM00;
	mSubdivision[2] = 0.0f;

	if (fabs(fM02) > 1e-6)
	{
		float fLength = sqrt(fM01 * fM01 + fM02 * fM02);
		float fInvLength = 1.0f / fLength;
		fM01 *= fInvLength;
		fM02 *= fInvLength;
		float fQ = 2.0f * fM01 * fM12 + fM02 * (fM22 - fM11);
		mDiagonal[1] = fM11 + fM02 * fQ;
		mDiagonal[2] = fM22 - fM02 * fQ;
		mSubdivision[0] = fLength;
		mSubdivision[1] = fM12 - fM01 * fQ;
		mMatrix[0][0] = 1.0f;
		mMatrix[0][1] = 0.0f;
		mMatrix[0][2] = 0.0f;
		mMatrix[1][0] = 0.0f;
		mMatrix[1][1] = fM01;
		mMatrix[1][2] = fM02;
		mMatrix[2][0] = 0.0f;
		mMatrix[2][1] = fM02;
		mMatrix[2][2] = -fM01;
		mIsRotation = false;
	}

	else
	{
		mDiagonal[1] = fM11;
		mDiagonal[2] = fM22;
		mSubdivision[0] = fM01;
		mSubdivision[1] = fM12;
		mMatrix[0][0] = 1.0f;
		mMatrix[0][1] = 0.0f;
		mMatrix[0][2] = 0.0f;
		mMatrix[1][0] = 0.0f;
		mMatrix[1][1] = 1.0f;
		mMatrix[1][2] = 0.0f;
		mMatrix[2][0] = 0.0f;
		mMatrix[2][1] = 0.0f;
		mMatrix[2][2] = 1.0f;
		mIsRotation = true;
	}
}

//////////////////////////////////////////////////////////////////////////
bool EigenSolver::QLAlgorithm()
{
	const int iMaxIter = 32;

	for (int i0 = 0; i0 < 3; i0++)
	{
		int i1;

		for (i1 = 0; i1 < iMaxIter; i1++)
		{
			int i2;

			for (i2 = i0; i2 <= 3 - 2; i2++)
			{
				float fTmp = fabs(mDiagonal[i2]) + fabs(mDiagonal[i2 + 1]);

				if (fabs(mSubdivision[i2]) + fTmp == fTmp)
				{
					break;
				}
			}

			if (i2 == i0)
			{
				break;
			}

			float fG = (mDiagonal[i0 + 1] - mDiagonal[i0]) / (2.0f *	mSubdivision[i0]);
			float fR = sqrt(fG * fG + 1.0f);

			if (fG < 0.0f)
			{
				fG = mDiagonal[i2] - mDiagonal[i0] + mSubdivision[i0] / (fG - fR);
			}

			else
			{
				fG = mDiagonal[i2] - mDiagonal[i0] + mSubdivision[i0] / (fG + fR);
			}

			float fSin = 1.0f, fCos = 1.0f, fP = 0.0f;

			for (int i3 = i2 - 1; i3 >= i0; i3--)
			{
				float fF = fSin * mSubdivision[i3];
				float fB = fCos * mSubdivision[i3];

				if (fabs(fF) >= fabs(fG))
				{
					fCos = fG / fF;
					fR = sqrt(fCos * fCos + 1.0f);
					mSubdivision[i3 + 1] = fF * fR;
					fSin = 1.0f / fR;
					fCos *= fSin;
				}

				else
				{
					fSin = fF / fG;
					fR = sqrt(fSin * fSin + 1.0f);
					mSubdivision[i3 + 1] = fG * fR;
					fCos = 1.0f / fR;
					fSin *= fCos;
				}

				fG = mDiagonal[i3 + 1] - fP;
				fR = (mDiagonal[i3] - fG) * fSin + 2.0f * fB * fCos;
				fP = fSin * fR;
				mDiagonal[i3 + 1] = fG + fP;
				fG = fCos * fR - fB;

				for (int i4 = 0; i4 < 3; i4++)
				{
					fF = mMatrix[i4][i3 + 1];
					mMatrix[i4][i3 + 1] = fSin * mMatrix[i4][i3] + fCos * fF;
					mMatrix[i4][i3] = fCos * mMatrix[i4][i3] - fSin * fF;
				}
			}

			mDiagonal[i0] -= fP;
			mSubdivision[i0] = fG;
			mSubdivision[i2] = 0.0f;
		}

		if (i1 == iMaxIter)
		{
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void EigenSolver::DecreasingSort()
{
	// sort eigenvalues in decreasing order, e[0] >= ... >= e[iSize-1]
	for (int i0 = 0, i1; i0 <= 3 - 2; i0++)
	{
		// locate maximum eigenvalue
		i1 = i0;
		float fMax = mDiagonal[i1];
		int i2;

		for (i2 = i0 + 1; i2 < 3; i2++)
		{
			if (mDiagonal[i2] > fMax)
			{
				i1 = i2;
				fMax = mDiagonal[i1];
			}
		}

		if (i1 != i0)
		{
			// swap eigenvalues
			mDiagonal[i1] = mDiagonal[i0];
			mDiagonal[i0] = fMax;

			// swap eigenvectors
			for (i2 = 0; i2 < 3; i2++)
			{
				float fTmp = mMatrix[i2][i0];
				mMatrix[i2][i0] = mMatrix[i2][i1];
				mMatrix[i2][i1] = fTmp;
				mIsRotation = !mIsRotation;
			}
		}
	}
}

void EigenSolver::IncreasingSort()
{
	// sort eigenvalues in increasing order, e[0] <= ... <= e[iSize-1]
	for (int i0 = 0, i1; i0 <= 3 - 2; i0++)
	{
		// locate minimum eigenvalue
		i1 = i0;
		float fMin = mDiagonal[i1];
		int i2;

		for (i2 = i0 + 1; i2 < 3; i2++)
		{
			if (mDiagonal[i2] < fMin)
			{
				i1 = i2;
				fMin = mDiagonal[i1];
			}
		}

		if (i1 != i0)
		{
			// swap eigenvalues
			mDiagonal[i1] = mDiagonal[i0];
			mDiagonal[i0] = fMin;

			// swap eigenvectors
			for (i2 = 0; i2 < 3; i2++)
			{
				float fTmp = mMatrix[i2][i0];
				mMatrix[i2][i0] = mMatrix[i2][i1];
				mMatrix[i2][i1] = fTmp;
				mIsRotation = !mIsRotation;
			}
		}
	}
}
