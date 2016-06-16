#ifndef COMMON_H_
#define COMMON_H_

#define srt_radian(x) x * 0.017453293f
#define srt_degree(x) x * 57.295779513f

#define srt_PI 3.14159265359f
#define srt_halfPI 1.57079632679f

#define srt_cast(x, T) dynamic_cast<T*>(x.get())
#define srt_is(x, T) dynamic_cast<T*>(x.get()) != 0

#define srt_max(a, b) (((a) > (b)) ? (a) : (b))
#define srt_min(a, b) (((a) < (b)) ? (a) : (b))

#define srt_clamp(v, vmin, vmax) ((v) < (vmin)) ? (vmin) : (((v) > (vmax)) ? (vmax) : (v))

#define srt_swap(a, b) \
		{ \
			float tmp = (a); \
			(a) = (b); \
			(b) = tmp; \
		}

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