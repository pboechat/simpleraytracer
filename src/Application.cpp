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

#include <windowsx.h>
#include <string>
#include <exception>

#include "Common.h"

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
const ColorRGBA Application::GLOBAL_AMBIENT_LIGHT(0.2f, 0.2f, 0.2f, 1);
const float Application::ANGLE_INCREMENT = 3;
const float Application::CAMERA_PITCH_LIMIT = 45;

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
	mDebugModeEnabled(false),
	mReloadScene(true),
	mLastSceneReloadTime(0),
	mToggleDebugMode(false),
	mLastDebugModeToggleTime(0),
	mpCommandPrompt(0),
	mRightMouseButtonPressed(false),
	mCameraYaw(0),
	mCameraPitch(0),
	mCameraRoll(0)
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
	mDebugModeEnabled = true;
	mToggleDebugMode = true;
}

//////////////////////////////////////////////////////////////////////////
void Application::DisableDebugMode()
{
	mDebugModeEnabled = false;
	mToggleDebugMode = true;
}

//////////////////////////////////////////////////////////////////////////
void Application::EnableRayDebugging(const Vector2F& rRayToDebug)
{
	unsigned int rayIndex = (unsigned int)rRayToDebug.y() * SCREEN_WIDTH + (unsigned int)rRayToDebug.x();
	mpOpenGLRenderer->EnableDebugRay(mpRayTracer->GetRaysMetadata()[rayIndex]);
	EnableDebugMode();
}

//////////////////////////////////////////////////////////////////////////
void Application::DisableRayDebugging()
{
	mpOpenGLRenderer->DisableDebugRay();
	DisableDebugMode();
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
					DisableDebugMode();

					LoadSceneFromXML();

					double start = Time::Now();

					mpRayTracer->SetScene(mpScene);
					mpOpenGLRenderer->SetScene(mpScene);

					double end = Time::Now();

					mReloadScene = false;
					mLastSceneReloadTime = Time::Now();

					mpCommandPrompt->ShowMessage("Elapsed time: %.5f seconds", (end - start));
				}
				else if (mToggleDebugMode && Time::Now() - mLastDebugModeToggleTime > 0.333f)
				{
					if (mDebugModeEnabled)
					{
						mpRenderer = mpOpenGLRenderer;
					}
					else
					{
						mpRenderer = mpRayTracer;
					}
					mpRenderer->SetScene(mpScene);

					mToggleDebugMode = false;
					mLastDebugModeToggleTime = Time::Now();
				}

				mpScene->Update();
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
	if (!mDebugModeEnabled)
	{
		return;
	}

	Camera* pCamera = mpScene->GetCamera();
	pCamera->localTransform.position -= pCamera->localTransform.right();
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraRight()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	Camera* pCamera = mpScene->GetCamera();
	pCamera->localTransform.position += pCamera->localTransform.right();
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraForward()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	Camera* pCamera = mpScene->GetCamera();
	pCamera->localTransform.position += pCamera->localTransform.forward();
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraBackward()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	Camera* pCamera = mpScene->GetCamera();
	pCamera->localTransform.position -= pCamera->localTransform.forward();
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraUp()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	Camera* pCamera = mpScene->GetCamera();
	pCamera->localTransform.position += Vector3F(0, 1, 0);
}

//////////////////////////////////////////////////////////////////////////
void Application::MoveCameraDown()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	Camera* pCamera = mpScene->GetCamera();
	pCamera->localTransform.position -= Vector3F(0, 1, 0);
}

//////////////////////////////////////////////////////////////////////////
void Application::KeyDown(unsigned int virtualKey)
{
	if (virtualKey == VK_ESCAPE)
	{
		DestroyWindow(mWindowHandle);
	}
	else if (virtualKey == VK_F5)
	{
		mReloadScene = true;
	}
	else if (virtualKey == VK_F2)
	{
		mpCommandPrompt->Toggle();
	}
	else if (virtualKey == VK_LEFT || virtualKey == 65)
	{
		MoveCameraLeft();
	}
	else if (virtualKey == VK_RIGHT || virtualKey == 68)
	{
		MoveCameraRight();
	}
	else if (virtualKey == VK_UP || virtualKey == 87)
	{
		MoveCameraForward();
	}
	else if (virtualKey == VK_DOWN || virtualKey == 83)
	{
		MoveCameraBackward();
	}
	else if (virtualKey == 81 || virtualKey == 33)
	{
		MoveCameraUp();
	}
	else if (virtualKey == 69 || virtualKey == 34)
	{
		MoveCameraDown();
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::KeyUp(unsigned int virtualKey)
{
	if (virtualKey == VK_F5)
	{
		mReloadScene = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::MouseButtonDown(unsigned int button, int x, int y)
{
	if (button == MK_RBUTTON)
	{
		mLastMousePosition = Vector2F((float)x, (float)y);
		mRightMouseButtonPressed = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::MouseButtonUp(unsigned int button, int x, int y)
{
	if (button == MK_RBUTTON)
	{
		mRightMouseButtonPressed = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void Application::MouseMove(int x, int y)
{
	if (!mRightMouseButtonPressed)
	{
		return;
	}

	if (mLastMousePosition.x() == x && mLastMousePosition.y() == y)
	{
		return;
	}

	Vector2F mousePosition = Vector2F((float)x, (float)y);
	Vector2F mouseDirection = (mousePosition - mLastMousePosition).Normalized();
	if (mouseDirection.x() > 0)
	{
		TurnCameraRight();
	}
	else if (mouseDirection.x() < 0)
	{
		TurnCameraLeft();
	}

	if (mouseDirection.y() > 0)
	{
		TurnCameraUp();
	}
	else if (mouseDirection.y() < 0)
	{
		TurnCameraDown();
	}

	mLastMousePosition = mousePosition;
}

//////////////////////////////////////////////////////////////////////////
void Application::TurnCameraUp()
{
	mCameraPitch = clamp(mCameraPitch - ANGLE_INCREMENT, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void Application::TurnCameraDown()
{
	mCameraPitch = clamp(mCameraPitch + ANGLE_INCREMENT, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void Application::TurnCameraLeft()
{
	mCameraYaw += ANGLE_INCREMENT;
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void Application::TurnCameraRight()
{
	mCameraYaw -= ANGLE_INCREMENT;
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void Application::UpdateCameraRotation()
{
	Vector3F forward = Matrix3F::AngleAxis(mCameraPitch, Vector3F(1, 0, 0)) * 
		Matrix3F::AngleAxis(mCameraYaw, Vector3F(0, 1, 0)) * Vector3F(0, 0, -1);

	Camera* pCamera = mpScene->GetCamera();
	Vector3F position = pCamera->localTransform.position;

	pCamera->localTransform.LookAt(position + forward);
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
	int x, y;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		Application::s_mpInstance->KeyDown(wParam);
		break;
	case WM_KEYUP:
		Application::s_mpInstance->KeyUp(wParam);
		break;
	case WM_LBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		Application::s_mpInstance->MouseButtonDown(MK_LBUTTON, x, y);
		break;
	case WM_MBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		Application::s_mpInstance->MouseButtonDown(MK_MBUTTON, x, y);
		break;
	case WM_RBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		Application::s_mpInstance->MouseButtonDown(MK_RBUTTON, x, y);
		break;
	case WM_LBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		Application::s_mpInstance->MouseButtonUp(MK_LBUTTON, x, y);
		break;
	case WM_MBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		Application::s_mpInstance->MouseButtonUp(MK_MBUTTON, x, y);
		break;
	case WM_RBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		Application::s_mpInstance->MouseButtonUp(MK_RBUTTON, x, y);
		break;
	case WM_MOUSEMOVE:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 

		Application::s_mpInstance->MouseMove(x, y);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
