#include "Application.h"
#include "Time.h"

#include "Sphere.h"
#include "PointLight.h"

#include <iostream>
#include <string>

const char* Application::WINDOW_TITLE = "simpleraytracer";
const char* Application::WINDOW_CLASS_NAME = "simpleraytracerWindowClass";
const unsigned int Application::WINDOW_WIDTH = 800;
const unsigned int Application::WINDOW_HEIGHT = 600;
const unsigned int Application::COLOR_BUFFER_BITS = 32;
const unsigned int Application::DEPTH_BUFFER_BITS = 32;
const unsigned int Application::HAS_ALPHA = 0;
const unsigned int Application::BYTES_PER_PIXEL = 4;
const unsigned int Application::COLOR_BUFFER_SIZE = WINDOW_WIDTH * WINDOW_HEIGHT * BYTES_PER_PIXEL;
const PIXELFORMATDESCRIPTOR Application::PIXEL_FORMAT_DESCRIPTOR = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, COLOR_BUFFER_BITS, 0, 0, 0, 0, 0, 0,	HAS_ALPHA, 0, 0, 0, 0, 0, 0, DEPTH_BUFFER_BITS, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

#ifdef _WIN32
PFNGLGENBUFFERSARBPROC pglGenBuffersARB = 0;
PFNGLBINDBUFFERARBPROC pglBindBufferARB = 0;
PFNGLBUFFERDATAARBPROC pglBufferDataARB = 0;
PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB = 0;
PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB = 0;
PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB = 0;
PFNGLMAPBUFFERARBPROC pglMapBufferARB = 0;
PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB = 0;
#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB
#endif

Application* Application::s_mpInstance = 0;

#define CHECK_WINAPI_SUCCESS(functionCall, errorMessage) \
	if (!functionCall) \
	{ \
		MessageBox(NULL, #errorMessage, WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION); \
		return EXIT_FAILURE; \
	} \

//////////////////////////////////////////////////////////////////////////
Application::Application() :
	mRunning(false),
	mApplicationHandle(0),
	mWindowHandle(0),
	mDeviceContextHandle(0),
	mPixelFormat(0),
	mOpenGLRenderingContextHandle(0),
	mTextureId(0),
	mPBOId(0),
	mpCamera(0),
	mpScene(0),
	mpRayTracer(0),
	mRunRayTracing(true),
	mLastRayTracingTime(0)
{
	s_mpInstance = this;
}

//////////////////////////////////////////////////////////////////////////
Application::~Application()
	
{
	s_mpInstance = 0;
}

void Application::CreateBuffers()
{
	bool PBOSupported = false;

#ifdef _WIN32
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	if(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB && glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
	{
		PBOSupported = true;
	}
#else
	// TODO:
#endif

	if (!PBOSupported)
	{
		// TODO:
	}

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffersARB(1, &mPBOId);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

//////////////////////////////////////////////////////////////////////////
int Application::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	mApplicationHandle = hInstance;

	CHECK_WINAPI_SUCCESS(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	CHECK_WINAPI_SUCCESS((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL)), "CreateWindow failed");
	CHECK_WINAPI_SUCCESS((mDeviceContextHandle = GetDC(mWindowHandle)), "GetDC() failed");
	CHECK_WINAPI_SUCCESS((mPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &PIXEL_FORMAT_DESCRIPTOR)), "ChoosePixelFormat() failed");
	CHECK_WINAPI_SUCCESS(SetPixelFormat(mDeviceContextHandle, mPixelFormat, &PIXEL_FORMAT_DESCRIPTOR), "SetPixelFormat() failed");
	CHECK_WINAPI_SUCCESS((mOpenGLRenderingContextHandle = wglCreateContext(mDeviceContextHandle)), "wglCreateContext() failed");
	CHECK_WINAPI_SUCCESS(wglMakeCurrent(mDeviceContextHandle, mOpenGLRenderingContextHandle), "wglMakeCurrent() failed");
	ShowWindow(mWindowHandle, SW_SHOW);
	SetForegroundWindow(mWindowHandle);
	SetFocus(mWindowHandle);

	InitializeOpenGL();
	CreateBuffers();

	mpCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, 45, 1, 100, Vector3F(0, 0, 0), Vector3F(0, 0, -1), Vector3F(0, 1, 0));

	Sphere* pSphere = new Sphere(Vector3F(0, 0, -5), 1);
	pSphere->material.diffuseColor = Color3F(1, 0, 0);
	pSphere->material.shininess = 5.0f;

	PointLight* pPointLight = new PointLight();
	pPointLight->position = Vector3F(1, 0, -3);

	mpScene = new Scene();
	mpScene->AddLight(pPointLight);
	mpScene->AddSceneObject(pSphere);

	mpRayTracer = new RayTracer(mpCamera, mpScene);
	
	mRunning = true;
	MSG message;
	while (mRunning)
	{

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				mRunning = false;
			}

			else
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}

		else
		{
			if (mRunRayTracing && Time::Now() - mLastRayTracingTime > 0.333f)
			{
				RunRayTracing();

				mRunRayTracing = false;
				mLastRayTracingTime = Time::Now();
			}

			RepaintWindow();
		}
	}

	delete mpRayTracer;
	delete mpScene;
	delete mpCamera;

	glDeleteTextures(1, &mTextureId);
	glDeleteBuffersARB(1, &mPBOId);
	CHECK_WINAPI_SUCCESS(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
	CHECK_WINAPI_SUCCESS(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
	CHECK_WINAPI_SUCCESS(UnregisterClass(WINDOW_CLASS_NAME, mApplicationHandle), "UnregisterClass() failed");

	return (int) message.wParam;
}

//////////////////////////////////////////////////////////////////////////
WNDCLASSEX Application::CreateWindowClass()
{
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProcedure;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = mApplicationHandle;
	windowClass.hIcon = LoadIcon(mApplicationHandle, MAKEINTRESOURCE(IDI_APPLICATION));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = WINDOW_CLASS_NAME;
	windowClass.hIconSm	= LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	return windowClass;
}

//////////////////////////////////////////////////////////////////////////
void Application::InitializeOpenGL()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

//////////////////////////////////////////////////////////////////////////
void Application::RepaintWindow()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// TODO: improve
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
		glTexCoord2f(0.0f, 0.0f);   
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);   
		glVertex3f( 1.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);   
		glVertex3f( 1.0f,  1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);   
		glVertex3f(-1.0f,  1.0f, 0.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	SwapBuffers(mDeviceContextHandle);
}

//////////////////////////////////////////////////////////////////////////
void Application::RunRayTracing()
{
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
	unsigned char* pColorBuffer = (unsigned char*) glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

	mpRayTracer->Render(pColorBuffer);

	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

//////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application application;
	return application.Run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_F5)
		{
			Application::s_mpInstance->mRunRayTracing = true;
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_F5)
		{
			Application::s_mpInstance->mRunRayTracing = false;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
