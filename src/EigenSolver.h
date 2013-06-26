#ifndef EIGENSOLVER_H_
#define EIGENSOLVER_H_

#include "Vector3F.h"
#include "Matrix3x3F.h"

class EigenSolver
{
public:
	EigenSolver(const Matrix3F& rMatrix);

	void Solve();
	void SortDescreasingly();
	void SortIncreasingly();
	float GetEigenValue(int i) const;
	Vector3F GetEigenVector(int i) const;

private:
	float mMatrix[3][3];
	float mDiagonal[3];
	float mSubdivision[3];
	bool mIsRotation;

	void GuaranteeRotation();
	void Tridiagonal();
	bool QLAlgorithm();
	void DecreasingSort();
	void IncreasingSort();

};

#endif
