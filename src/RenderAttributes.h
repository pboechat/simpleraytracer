#ifndef RENDERATTRIBUTES_H_
#define RENDERATTRIBUTES_H_

struct RenderAttributes
{
	bool transparent;
	float reflectionCoeficient;
	float refractionIndex;

	RenderAttributes() :
		transparent(false),
		reflectionCoeficient(0),
		refractionIndex(0)
	{
	}

	~RenderAttributes()
	{
	}

};

#endif