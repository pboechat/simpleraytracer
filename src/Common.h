#ifndef COMMON_H_
#define COMMON_H_

#define degree(x) x * 0.017453293f

#define cast(x, y) dynamic_cast<y*>(x)
#define is(x, y) dynamic_cast<y*>(x) != 0

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define clamp(v, vmin, vmax) (v) = ((v) < (vmin)) ? (vmin) : (((v) > (vmax)) ? (vmax) : (v))

#if defined(__gl_h_) && defined(__glu_h__) && defined(_EXCEPTION_)

#define checkOpenGL() \
{ \
	unsigned int __errorCode; \
	if ((__errorCode = glGetError()) != GL_NO_ERROR) \
	{ \
	const unsigned char* __pErrorMessage = gluErrorString(__errorCode); \
	char buffer[1024]; \
	if (__pErrorMessage != 0) \
		{ \
		sprintf(buffer, "%s (code: %d)", __pErrorMessage, __errorCode); \
		} \
		throw std::exception(buffer); \
	} \
} \

#endif

#endif