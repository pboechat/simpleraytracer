#include "Application.h"
#include "Time.h"

#include "Vector3F.h"
#include "ColorRGBA.h"
#include "Sphere.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "PointLight.h"
#include "TextureLoader.h"
#include "SceneLoader.h"

#include <string>
#include <exception>

const char* Application::WINDOW_TITLE = "simpleraytracer";
const char* Application::WINDOW_CLASS_NAME = "simpleraytracerWindowClass";
const unsigned int Application::WINDOW_WIDTH = 800;
const unsigned int Application::WINDOW_HEIGHT = 600;
const unsigned int Application::COLOR_BUFFER_BITS = 32;
const unsigned int Application::DEPTH_BUFFER_BITS = 32;
const unsigned int Application::HAS_ALPHA = 0;
const unsigned int Application::BYTES_PER_PIXEL = 4;
const unsigned int Application::DEPTH_BUFFER_SIZE = WINDOW_WIDTH * WINDOW_HEIGHT;
const unsigned int Application::COLOR_BUFFER_SIZE = DEPTH_BUFFER_SIZE * BYTES_PER_PIXEL;
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
		Dispose(); \
		exit(EXIT_FAILURE); \
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
	mpSceneFileName(0),
	mpCamera(0),
	mpScene(0),
	mpRayTracer(0),
	mpOpenGLRenderer(0),
	mPBOSupported(false),
	mpTextureData(0),
	mpDepthBuffer(0),
	mReloadScene(true),
	mLastSceneReloadTime(0),
	mClearColor(0, 0, 0, 0),
	mpCommandPrompt(0),
	mDebugModeEnabled(false),
	mRayDebuggingEnabled(false)
{
	s_mpInstance = this;
}

//////////////////////////////////////////////////////////////////////////
Application::~Application()
	
{
	s_mpInstance = 0;
}

//////////////////////////////////////////////////////////////////////////
void Application::CreateBuffers()
{
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
		mPBOSupported = true;
	}
#else
	// TODO:
#endif

	if (mPBOSupported)
	{
		glGenBuffersARB(1, &mPBOId);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		mpTextureData = 0;
	}
	else
	{
		mpTextureData = new unsigned char[COLOR_BUFFER_SIZE];
		memset(mpTextureData, 0, sizeof(unsigned char) * COLOR_BUFFER_SIZE);
	}

	mpDepthBuffer = new float[DEPTH_BUFFER_SIZE];

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////
int Application::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	mApplicationHandle = hInstance;

	CHECK_WINAPI_SUCCESS(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	CHECK_WINAPI_SUCCESS((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN), CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL)), "CreateWindow failed");
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
	
	mpCamera = new Camera();
	mpScene = new Scene();

	mpSceneFileName = lpCmdLine; 

	mpRayTracer = new RayTracer(mpCamera, mpScene, mClearColor, ColorRGBA(0.5f, 0.5f, 0.5f, 1));
	
	mpOpenGLRenderer = new OpenGLRenderer(mpCamera, mpScene);

	mpCommandPrompt = new CommandPrompt();
	mpCommandPrompt->Start();

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
			if (mReloadScene && Time::Now() - mLastSceneReloadTime > 0.333f)
			{
				LoadSceneFromXML();

				double start = Time::Now();

				RunRayTracing();

				double end = Time::Now();

				mReloadScene = false;
				mLastSceneReloadTime = Time::Now();

				mpCommandPrompt->ShowMessage("Elapsed time: %.5f seconds", (end - start));
			}

			RepaintWindow();
		}
	}

	Dispose();

	CHECK_WINAPI_SUCCESS(UnregisterClass(WINDOW_CLASS_NAME, mApplicationHandle), "UnregisterClass() failed");

	mDeviceContextHandle = 0;
	mWindowHandle = 0;
	mApplicationHandle = 0;

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
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

//////////////////////////////////////////////////////////////////////////
void Application::RepaintWindow()
{
	if (mDebugModeEnabled)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(mpCamera->GetProjectionMatrix()[0]);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(mpCamera->GetViewMatrix()[0]);

		glClearColor(mClearColor.r(), mClearColor.g(), mClearColor.b(), mClearColor.a());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mpOpenGLRenderer->Render();
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBindTexture(GL_TEXTURE_2D, mTextureId);
		if (mPBOSupported)
		{
			glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData);

		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, mTextureId);
		glColor4f(1, 1, 1, 1);

		// TODO: improve
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);   
			glNormal3f(0, 0, 1);
			glVertex3f(-1, -1, 0);

			glTexCoord2f(1, 0);   
			glNormal3f(0, 0, 1);
			glVertex3f(1, -1, 0);

			glTexCoord2f(1, 1);   
			glNormal3f(0, 0, 1);
			glVertex3f(1, 1, 0);

			glTexCoord2f(0, 1); 
			glNormal3f(0, 0, 1);
			glVertex3f(-1, 1, 0);
		glEnd();
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	SwapBuffers(mDeviceContextHandle);
}

//////////////////////////////////////////////////////////////////////////
void Application::RunRayTracing()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	if (mPBOSupported)
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData);

	for (unsigned int i = 0; i < DEPTH_BUFFER_SIZE; i++)
	{
		mpDepthBuffer[i] = mpCamera->GetFar();
	}

	if (mPBOSupported)
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
		unsigned char* pColorBuffer = (unsigned char*) glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

		if (pColorBuffer != 0)
		{
			mpRayTracer->Render(pColorBuffer, mpDepthBuffer);
			glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
		}

		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}
	else
	{
		mpRayTracer->Render(mpTextureData, mpDepthBuffer);
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::Dispose()
{
	if (mpRayTracer)
	{
		delete mpRayTracer;
		mpRayTracer = 0;
	}

	if (mpOpenGLRenderer)
	{
		delete mpOpenGLRenderer;
		mpOpenGLRenderer = 0;
	}

	if (mpCommandPrompt)
	{
		mpCommandPrompt->Finish();
		delete mpCommandPrompt;
		mpCommandPrompt = 0;
	}
	
	if (mpScene)
	{
		delete mpScene;
		mpScene = 0;
	}
	
	if (mpCamera)
	{
		delete mpCamera;
		mpCamera = 0;
	}

	if (mTextureId)
	{
		glDeleteTextures(1, &mTextureId);
		mTextureId = 0;
	}
	
	if (mPBOId)
	{
		glDeleteBuffersARB(1, &mPBOId);
		mPBOId = 0;
	}
	
	if (mOpenGLRenderingContextHandle)
	{
		CHECK_WINAPI_SUCCESS(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
		CHECK_WINAPI_SUCCESS(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
		mOpenGLRenderingContextHandle = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::LoadSceneFromXML()
{
	mpScene->Clear();
	try
	{
		SceneLoader::LoadFromXML(mpSceneFileName, *mpScene, *mpCamera);
	}
	catch (std::exception& rException)
	{
		MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		Dispose();
		exit(EXIT_FAILURE);
	}
	mpCamera->SetViewport(WINDOW_WIDTH, WINDOW_HEIGHT);
	mpScene->Update();
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
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(Application::s_mpInstance->mWindowHandle);
		}
		else if (wParam == VK_F5)
		{
			Application::s_mpInstance->mReloadScene = true;
		}
		else if (wParam == VK_F2)
		{
			Application::s_mpInstance->mpCommandPrompt->Toggle();
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_F5)
		{
			Application::s_mpInstance->mReloadScene = false;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
