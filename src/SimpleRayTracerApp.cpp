#include "SimpleRayTracerApp.h"
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

SimpleRayTracerApp* SimpleRayTracerApp::s_mpInstance = 0;
const char* SimpleRayTracerApp::WINDOW_TITLE = "simpleraytracer";
const char* SimpleRayTracerApp::WINDOW_CLASS_NAME = "simpleraytracerWindowClass";
const unsigned int SimpleRayTracerApp::SCREEN_WIDTH = 800;
const unsigned int SimpleRayTracerApp::SCREEN_HEIGHT = 600;
const unsigned int SimpleRayTracerApp::BYTES_PER_PIXEL = 4;
const unsigned int SimpleRayTracerApp::COLOR_BUFFER_BITS = 32;
const unsigned int SimpleRayTracerApp::DEPTH_BUFFER_BITS = 32;
const unsigned int SimpleRayTracerApp::HAS_ALPHA = 0;
const PIXELFORMATDESCRIPTOR SimpleRayTracerApp::PIXEL_FORMAT_DESCRIPTOR = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, COLOR_BUFFER_BITS, 0, 0, 0, 0, 0, 0,	HAS_ALPHA, 0, 0, 0, 0, 0, 0, DEPTH_BUFFER_BITS, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
const ColorRGBA SimpleRayTracerApp::CLEAR_COLOR(0, 0, 0, 1);
const ColorRGBA SimpleRayTracerApp::GLOBAL_AMBIENT_LIGHT(0.2f, 0.2f, 0.2f, 1);
const float SimpleRayTracerApp::ANGLE_INCREMENT = 3;
const float SimpleRayTracerApp::CAMERA_PITCH_LIMIT = 45;

//////////////////////////////////////////////////////////////////////////
SimpleRayTracerApp::SimpleRayTracerApp() :
	mRunning(false),
	mApplicationHandle(0),
	mWindowHandle(0),
	mDeviceContextHandle(0),
	mPixelFormat(0),
	mOpenGLRenderingContextHandle(0),
	mpSceneFileName(0),
	mScene(nullptr),
	mRayTracer(0),
	mOpenGLRenderer(0),
	mRenderer(0),
	mDebugModeEnabled(false),
	mReloadScene(true),
	mLastSceneReloadTime(0),
	mToggleDebugMode(false),
	mLastDebugModeToggleTime(0),
	//mCommandPrompt(nullptr),
	mRightMouseButtonPressed(false),
	mCameraYaw(0),
	mCameraPitch(0),
	mCameraRoll(0)
{
	s_mpInstance = this;
}

//////////////////////////////////////////////////////////////////////////
SimpleRayTracerApp::~SimpleRayTracerApp()
{
	s_mpInstance = nullptr;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::EnableDebugMode()
{
	mDebugModeEnabled = true;
	mToggleDebugMode = true;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::DisableDebugMode()
{
	mDebugModeEnabled = false;
	mToggleDebugMode = true;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::EnableRayDebugging(const Vector2F& rRayToDebug)
{
	unsigned int rayIndex = (unsigned int)rRayToDebug.y() * SCREEN_WIDTH + (unsigned int)rRayToDebug.x();
	mOpenGLRenderer->EnableDebugRay(mRayTracer->GetRaysMetadata()[rayIndex]);
	EnableDebugMode();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::DisableRayDebugging()
{
	mOpenGLRenderer->DisableDebugRay();
	DisableDebugMode();
}

//////////////////////////////////////////////////////////////////////////
int SimpleRayTracerApp::Run(unsigned int argc, const char** argv)
{
	if (argc < 2)
	{
		MessageBox(0, "no scene file specified", WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		exit(EXIT_FAILURE);
	}

	mApplicationHandle = GetModuleHandle(0);

	win32Assert(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	win32Assert((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN), CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, mApplicationHandle, NULL)), "CreateWindow failed");
	win32Assert((mDeviceContextHandle = GetDC(mWindowHandle)), "GetDC() failed");
	win32Assert((mPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &PIXEL_FORMAT_DESCRIPTOR)), "ChoosePixelFormat() failed");
	win32Assert(SetPixelFormat(mDeviceContextHandle, mPixelFormat, &PIXEL_FORMAT_DESCRIPTOR), "SetPixelFormat() failed");
	win32Assert((mOpenGLRenderingContextHandle = wglCreateContext(mDeviceContextHandle)), "wglCreateContext() failed");
	win32Assert(wglMakeCurrent(mDeviceContextHandle, mOpenGLRenderingContextHandle), "wglMakeCurrent() failed");
	ShowWindow(mWindowHandle, SW_SHOW);
	SetForegroundWindow(mWindowHandle);
	SetFocus(mWindowHandle);

	mpSceneFileName = argv[1]; 

	mRayTracer = std::shared_ptr<RayTracer>(new RayTracer());
	mOpenGLRenderer = std::shared_ptr<OpenGLRenderer>(new OpenGLRenderer());

	mRayTracer->Start();
	mOpenGLRenderer->Start();

	mRenderer = mRayTracer; // starting renderer

	//mCommandPrompt = std::unique_ptr<CommandPrompt>(new CommandPrompt());
	//mCommandPrompt->Start();

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

					mRayTracer->SetScene(mScene);
					mOpenGLRenderer->SetScene(mScene);

					double end = Time::Now();

					mReloadScene = false;
					mLastSceneReloadTime = Time::Now();

					//mCommandPrompt->ShowMessage("Elapsed time: %.5f seconds", (end - start));
				}
				else if (mToggleDebugMode && Time::Now() - mLastDebugModeToggleTime > 0.333f)
				{
					if (mDebugModeEnabled)
					{
						mRenderer = mOpenGLRenderer;
					}
					else
					{
						mRenderer = mRayTracer;
					}
					mRenderer->SetScene(mScene);

					mToggleDebugMode = false;
					mLastDebugModeToggleTime = Time::Now();
				}

				mScene->Update();
				mRenderer->Render();
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
WNDCLASSEX SimpleRayTracerApp::CreateWindowClass()
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
void SimpleRayTracerApp::Dispose()
{
	mRayTracer = nullptr;
	mOpenGLRenderer = nullptr;
	mRenderer = nullptr;
	mScene = nullptr;

	/*if (mCommandPrompt)
	{
		mCommandPrompt->Terminate();
		mCommandPrompt = nullptr;
	}*/
	
	if (mOpenGLRenderingContextHandle)
	{
		win32Assert(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
		win32Assert(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
		mOpenGLRenderingContextHandle = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::LoadSceneFromXML()
{
	try
	{
		mScene = SceneLoader::LoadFromXML(mpSceneFileName);
	}
	catch (std::exception& rException)
	{
		MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		Dispose();
		exit(EXIT_FAILURE);
	}
	mScene->Update();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraLeft()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	camera->localTransform.position -= camera->localTransform.right();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraRight()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.right();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraForward()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.forward();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraBackward()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	camera->localTransform.position -= camera->localTransform.forward();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraUp()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	camera->localTransform.position += Vector3F(0, 1, 0);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraDown()
{
	if (!mDebugModeEnabled)
	{
		return;
	}

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	camera->localTransform.position -= Vector3F(0, 1, 0);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::KeyDown(unsigned int virtualKey)
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
		//mCommandPrompt->Toggle();
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
void SimpleRayTracerApp::KeyUp(unsigned int virtualKey)
{
	if (virtualKey == VK_F5)
	{
		mReloadScene = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MouseButtonDown(unsigned int button, int x, int y)
{
	if (button == MK_RBUTTON)
	{
		mLastMousePosition = Vector2F((float)x, (float)y);
		mRightMouseButtonPressed = true;
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MouseButtonUp(unsigned int button, int x, int y)
{
	if (button == MK_RBUTTON)
	{
		mRightMouseButtonPressed = false;
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MouseMove(int x, int y)
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
void SimpleRayTracerApp::TurnCameraUp()
{
	mCameraPitch = clamp(mCameraPitch - ANGLE_INCREMENT, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::TurnCameraDown()
{
	mCameraPitch = clamp(mCameraPitch + ANGLE_INCREMENT, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::TurnCameraLeft()
{
	mCameraYaw += ANGLE_INCREMENT;
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::TurnCameraRight()
{
	mCameraYaw -= ANGLE_INCREMENT;
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::UpdateCameraRotation()
{
	Vector3F forward = Matrix3F::AngleAxis(mCameraPitch, Vector3F(1, 0, 0)) * 
		Matrix3F::AngleAxis(mCameraYaw, Vector3F(0, 1, 0)) * Vector3F(0, 0, -1);

	std::unique_ptr<Camera>& camera = mScene->GetCamera();
	Vector3F position = camera->localTransform.position;
	camera->localTransform.LookAt(position + forward);
}

void SimpleRayTracerApp::Close()
{
	
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
		SimpleRayTracerApp::s_mpInstance->KeyDown(wParam);
		break;
	case WM_KEYUP:
		SimpleRayTracerApp::s_mpInstance->KeyUp(wParam);
		break;
	case WM_LBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SimpleRayTracerApp::s_mpInstance->MouseButtonDown(MK_LBUTTON, x, y);
		break;
	case WM_MBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SimpleRayTracerApp::s_mpInstance->MouseButtonDown(MK_MBUTTON, x, y);
		break;
	case WM_RBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SimpleRayTracerApp::s_mpInstance->MouseButtonDown(MK_RBUTTON, x, y);
		break;
	case WM_LBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SimpleRayTracerApp::s_mpInstance->MouseButtonUp(MK_LBUTTON, x, y);
		break;
	case WM_MBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SimpleRayTracerApp::s_mpInstance->MouseButtonUp(MK_MBUTTON, x, y);
		break;
	case WM_RBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SimpleRayTracerApp::s_mpInstance->MouseButtonUp(MK_RBUTTON, x, y);
		break;
	case WM_MOUSEMOVE:
		x = GET_X_LPARAM(lParam); 
		y = GET_Y_LPARAM(lParam); 

		SimpleRayTracerApp::s_mpInstance->MouseMove(x, y);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
