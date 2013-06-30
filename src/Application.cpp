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

#define win32Assert(resultHandle, errorMessage) \
	if (resultHandle == 0) \
	{ \
		MessageBox(NULL, #errorMessage, WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION); \
		Dispose(); \
		exit(EXIT_FAILURE); \
	} \

Application* Application::s_mpInstance = 0;
const char* Application::WINDOW_TITLE = "simpleraytracer";
const char* Application::WINDOW_CLASS_NAME = "simpleraytracerWindowClass";
const unsigned int Application::SCREEN_WIDTH = 800;
const unsigned int Application::SCREEN_HEIGHT = 600;
const unsigned int Application::BYTES_PER_PIXEL = 4;
const unsigned int Application::COLOR_BUFFER_BITS = 32;
const unsigned int Application::DEPTH_BUFFER_BITS = 32;
const unsigned int Application::HAS_ALPHA = 0;
const PIXELFORMATDESCRIPTOR Application::PIXEL_FORMAT_DESCRIPTOR = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, COLOR_BUFFER_BITS, 0, 0, 0, 0, 0, 0,	HAS_ALPHA, 0, 0, 0, 0, 0, 0, DEPTH_BUFFER_BITS, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
const ColorRGBA Application::CLEAR_COLOR(0, 0, 0, 1);
const ColorRGBA Application::GLOBAL_AMBIENT_LIGHT(0.5f, 0.5f, 0.5f, 1);

//////////////////////////////////////////////////////////////////////////
Application::Application() :
	mRunning(false),
	mApplicationHandle(0),
	mWindowHandle(0),
	mDeviceContextHandle(0),
	mPixelFormat(0),
	mOpenGLRenderingContextHandle(0),
	mpSceneFileName(0),
	mpScene(0),
	mpRayTracer(0),
	mpOpenGLRenderer(0),
	mpRenderer(0),
	mReloadScene(true),
	mLastSceneReloadTime(0),
	mpCommandPrompt(0)
{
	s_mpInstance = this;
}

//////////////////////////////////////////////////////////////////////////
Application::~Application()
	
{
	s_mpInstance = 0;
}

//////////////////////////////////////////////////////////////////////////
void Application::EnableDebugMode()
{
	mpRenderer = mpOpenGLRenderer;
}

//////////////////////////////////////////////////////////////////////////
void Application::DisableDebugMode()
{
	mpRenderer = mpRayTracer;
}

//////////////////////////////////////////////////////////////////////////
void Application::EnableRayDebugging(const Vector2F& rRayToDebug)
{
	unsigned int rayIndex = (unsigned int)rRayToDebug.y() * SCREEN_WIDTH + (unsigned int)rRayToDebug.x();
	mpOpenGLRenderer->EnableDebugRay(mpRayTracer->GetRaysMetadata()[rayIndex]);
	mpRenderer = mpOpenGLRenderer;
}

//////////////////////////////////////////////////////////////////////////
void Application::DisableRayDebugging()
{
	mpOpenGLRenderer->DisableDebugRay();
	mpRenderer = mpRayTracer;
}

//////////////////////////////////////////////////////////////////////////
int Application::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	mApplicationHandle = hInstance;

	win32Assert(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	win32Assert((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN), CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL)), "CreateWindow failed");
	win32Assert((mDeviceContextHandle = GetDC(mWindowHandle)), "GetDC() failed");
	win32Assert((mPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &PIXEL_FORMAT_DESCRIPTOR)), "ChoosePixelFormat() failed");
	win32Assert(SetPixelFormat(mDeviceContextHandle, mPixelFormat, &PIXEL_FORMAT_DESCRIPTOR), "SetPixelFormat() failed");
	win32Assert((mOpenGLRenderingContextHandle = wglCreateContext(mDeviceContextHandle)), "wglCreateContext() failed");
	win32Assert(wglMakeCurrent(mDeviceContextHandle, mOpenGLRenderingContextHandle), "wglMakeCurrent() failed");
	ShowWindow(mWindowHandle, SW_SHOW);
	SetForegroundWindow(mWindowHandle);
	SetFocus(mWindowHandle);

	mpSceneFileName = lpCmdLine; 

	mpRayTracer = new RayTracer();
	mpOpenGLRenderer = new OpenGLRenderer();

	mpRayTracer->Start();
	mpOpenGLRenderer->Start();

	mpRenderer = mpRayTracer; // starting renderer

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
			try
			{
				if (mReloadScene && Time::Now() - mLastSceneReloadTime > 0.333f)
				{
					LoadSceneFromXML();

					double start = Time::Now();

					mpRayTracer->SetScene(mpScene);
					mpOpenGLRenderer->SetScene(mpScene);

					double end = Time::Now();

					mReloadScene = false;
					mLastSceneReloadTime = Time::Now();

					mpCommandPrompt->ShowMessage("Elapsed time: %.5f seconds", (end - start));
				}

				mpRenderer->Render();
				SwapBuffers(mDeviceContextHandle);
			}
			catch (std::exception& rException)
			{
				MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
				DestroyWindow(mWindowHandle);
			}
		}
	}

	Dispose();

	win32Assert(UnregisterClass(WINDOW_CLASS_NAME, mApplicationHandle), "UnregisterClass() failed");

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

	mpRenderer = 0;
	
	if (mpScene)
	{
		delete mpScene;
		mpScene = 0;
	}

	if (mpCommandPrompt)
	{
		mpCommandPrompt->Finish();
		delete mpCommandPrompt;
		mpCommandPrompt = 0;
	}
	
	if (mOpenGLRenderingContextHandle)
	{
		win32Assert(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
		win32Assert(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
		mOpenGLRenderingContextHandle = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::LoadSceneFromXML()
{
	if (mpScene != 0)
	{
		delete mpScene;
	}

	try
	{
		mpScene = SceneLoader::LoadFromXML(mpSceneFileName);
	}
	catch (std::exception& rException)
	{
		MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		Dispose();
		exit(EXIT_FAILURE);
	}
	mpScene->Update();
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraLeft()
{
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraRight()
{
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
		else if (wParam == VK_LEFT)
		{
			Application::s_mpInstance->MoveCameraLeft();
		}
		else if (wParam == VK_RIGHT)
		{
			Application::s_mpInstance->MoveCameraRight();
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
