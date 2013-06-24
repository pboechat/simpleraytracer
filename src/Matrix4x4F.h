#ifndef MATRIX4X4F_H_
#define MATRIX4X4F_H_

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
	inline const float* operator[] (unsigned int i) const
	{
		return &mMatrix[0];
	}

private:
	float mMatrix[16];

};

typedef Matrix4x4F Matrix4F;

#endif