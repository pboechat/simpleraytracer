#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "Camera.h"
#include "Scene.h"
#include "RayTracer.h"
#include "OpenGLRenderer.h"
#include "CommandPrompt.h"
#include "ColorRGBA.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"

//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
class Application
{
public:
	inline static Application* GetInstance()
	{
		return s_mpInstance;
	}

	int Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	inline void EnableDebugMode()
	{
		mDebugModeEnabled = true;
	}

	inline void DisableDebugMode()
	{
		mDebugModeEnabled = false;
	}

	inline void EnableRayDebugging(const Vector2F& rRayToDebug)
	{
		mDebugModeEnabled = true;
		mRayToDebug = rRayToDebug;
		mRayDebuggingEnabled = true;
	}

	inline void DisableRayDebugging()
	{
		mRayDebuggingEnabled = false;
		mDebugModeEnabled = false;
	}

	friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	friend LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static const char* WINDOW_TITLE;
	static const char* WINDOW_CLASS_NAME;
	static const unsigned int WINDOW_WIDTH;
	static const unsigned int WINDOW_HEIGHT;
	static const unsigned int COLOR_BUFFER_BITS;
	static const unsigned int DEPTH_BUFFER_BITS;
	static const unsigned int HAS_ALPHA;
	static const unsigned int BYTES_PER_PIXEL;
	static const unsigned int COLOR_BUFFER_SIZE;
	static const unsigned int DEPTH_BUFFER_SIZE;
	static const PIXELFORMATDESCRIPTOR PIXEL_FORMAT_DESCRIPTOR;

	static Application* s_mpInstance;

	bool mRunning;
	HINSTANCE mApplicationHandle;
	HWND mWindowHandle;
	HDC mDeviceContextHandle;
	int mPixelFormat;
	HGLRC mOpenGLRenderingContextHandle;
	unsigned int mTextureId;
	unsigned int mPBOId;
	char* mpSceneFileName;
	Camera* mpCamera;
	Scene* mpScene;
	RayTracer* mpRayTracer;
	OpenGLRenderer* mpOpenGLRenderer;
	bool mPBOSupported;
	unsigned char* mpTextureData;
	float* mpDepthBuffer;
	bool mReloadScene;
	double mLastSceneReloadTime;
	ColorRGBA mClearColor;
	CommandPrompt* mpCommandPrompt;
	bool mDebugModeEnabled;
	bool mRayDebuggingEnabled;
	Vector2F mRayToDebug;
	
	Application();
	~Application();

	void InitializeOpenGL();
	void CreateBuffers();
	void CheckCommandPrompt();
	void RepaintWindow();
	void LoadSceneFromXML();
	void RunRayTracing();
	void Dispose();
	WNDCLASSEX CreateWindowClass();

};


#endif