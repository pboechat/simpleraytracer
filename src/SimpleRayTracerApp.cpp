#include <windowsx.h>
#include <string>
#include <cassert>
#include <stdexcept>

#include "Common.h"
#include "SimpleRayTracerApp.h"
#include "Vector3F.h"
#include "ColorRGBA.h"
#include "Sphere.h"
#include "Mesh.h"
#include "ModelLoader.h"
#include "PointLight.h"
#include "TextureLoader.h"
#include "SceneLoader.h"
#include "Time.h"

#define srt_win32Assert(resultHandle, errorMessage) \
	if (resultHandle == 0) \
	{ \
		MessageBox(NULL, #errorMessage, WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION); \
		Dispose(); \
		exit(EXIT_FAILURE); \
	} \

SimpleRayTracerApp* SimpleRayTracerApp::s_mpInstance = 0;
const char* SimpleRayTracerApp::WINDOW_TITLE = "simpleraytracer";
const char* SimpleRayTracerApp::WINDOW_CLASS_NAME = "simpleraytracerWindowClass";
const unsigned int SimpleRayTracerApp::SCREEN_WIDTH = 1024;
const unsigned int SimpleRayTracerApp::SCREEN_HEIGHT = 768;
const unsigned int SimpleRayTracerApp::BYTES_PER_PIXEL = 4;
const unsigned int SimpleRayTracerApp::COLOR_BUFFER_BITS = 32;
const unsigned int SimpleRayTracerApp::DEPTH_BUFFER_BITS = 32;
const unsigned int SimpleRayTracerApp::HAS_ALPHA = 0;
const PIXELFORMATDESCRIPTOR SimpleRayTracerApp::PIXEL_FORMAT_DESCRIPTOR = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, COLOR_BUFFER_BITS, 0, 0, 0, 0, 0, 0,	HAS_ALPHA, 0, 0, 0, 0, 0, 0, DEPTH_BUFFER_BITS, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
const ColorRGBA SimpleRayTracerApp::CLEAR_COLOR(0, 0, 0, 1);
const ColorRGBA SimpleRayTracerApp::GLOBAL_AMBIENT_LIGHT(0.2f, 0.2f, 0.2f, 1);
const float SimpleRayTracerApp::ANGLE_INCREMENT = 0.05f;
const float SimpleRayTracerApp::CAMERA_PITCH_LIMIT = 1.0472f; // 60 deg.
const float SimpleRayTracerApp::CAMERA_MOVE_SPEED = 10.0f;
const float SimpleRayTracerApp::DEBUG_RAY_MOVE_SPEED = 1000.0f;

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
	mLoadScene(true),
	mRightMouseButtonPressed(false),
	mLastMousePosition(-1, -1),
	mCameraPhi(0),
	mCameraTheta(0),
	mDebugRayCoords(0, 0)
{
	memset(mKeys, 0, sizeof(bool) * 0xFF);
	memset(mPressedKeys, 0, sizeof(bool) * 0xFF);
	s_mpInstance = this;
}

//////////////////////////////////////////////////////////////////////////
SimpleRayTracerApp::~SimpleRayTracerApp()
{
	s_mpInstance = nullptr;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::EnableRayDebugging()
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	mOpenGLRenderer->EnableDebugRay();
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::DisableRayDebugging()
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	mOpenGLRenderer->DisableDebugRay();
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

	srt_win32Assert(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	srt_win32Assert((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN), CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, mApplicationHandle, NULL)), "CreateWindow failed");
	srt_win32Assert((mDeviceContextHandle = GetDC(mWindowHandle)), "GetDC() failed");
	srt_win32Assert((mPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &PIXEL_FORMAT_DESCRIPTOR)), "ChoosePixelFormat() failed");
	srt_win32Assert(SetPixelFormat(mDeviceContextHandle, mPixelFormat, &PIXEL_FORMAT_DESCRIPTOR), "SetPixelFormat() failed");
	srt_win32Assert((mOpenGLRenderingContextHandle = wglCreateContext(mDeviceContextHandle)), "wglCreateContext() failed");
	srt_win32Assert(wglMakeCurrent(mDeviceContextHandle, mOpenGLRenderingContextHandle), "wglMakeCurrent() failed");
	ShowWindow(mWindowHandle, SW_SHOW);
	SetForegroundWindow(mWindowHandle);
	SetFocus(mWindowHandle);

	mpSceneFileName = argv[1]; 

	mRayTracer = std::shared_ptr<RayTracer>(new RayTracer());
	mOpenGLRenderer = std::shared_ptr<OpenGLRenderer>(new OpenGLRenderer());

	mRayTracer->Start();
	mOpenGLRenderer->Start();

	mRenderer = mRayTracer;

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
				auto start = Time::Now();
				if (mLoadScene)
				{
					LoadSceneFromXML();
					mRayTracer->SetScene(mScene);
					mOpenGLRenderer->SetScene(mScene);
					mLoadScene = false;
				}
				mScene->Update();
				mRenderer->Render();
				auto end = Time::Now();
				float deltaTime = static_cast<float>(end - start);
				ProcessKeys(deltaTime);
				memset(mPressedKeys, 0, sizeof(bool) * 0xFF);
				SwapBuffers(mDeviceContextHandle);
			}
			catch (std::runtime_error& rException)
			{
				MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
				DestroyWindow(mWindowHandle);
			}
		}
	}

	Dispose();

	srt_win32Assert(UnregisterClass(WINDOW_CLASS_NAME, mApplicationHandle), "UnregisterClass() failed");

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
	
	if (mOpenGLRenderingContextHandle)
	{
		srt_win32Assert(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
		srt_win32Assert(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
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
	catch (std::runtime_error& rException)
	{
		MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		Dispose();
		exit(EXIT_FAILURE);
	}
	mScene->Update();

	auto& camera = mScene->GetCamera();
	auto forward = camera->localTransform.forward();
	mCameraPhi = srt_halfPI - atan2(forward.x(), forward.z());
	mCameraTheta = srt_halfPI - atan2(hypot(forward.x(), forward.z()), forward.y());
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraLeft(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position -= camera->localTransform.right() * deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraRight(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.right() * deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraForward(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position -= camera->localTransform.forward() * deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraBackward(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.forward() * deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraUp(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position += Vector3F(0, deltaTime * CAMERA_MOVE_SPEED, 0);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraDown(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position -= Vector3F(0, deltaTime * CAMERA_MOVE_SPEED, 0);
}

//////////////////////////////////////////////////////////////////////////
bool SimpleRayTracerApp::IsRayTracingEnabled() const
{
	return mRenderer == mRayTracer;
}

void SimpleRayTracerApp::ProcessKeys(float deltaTime)
{
	if (mPressedKeys[VK_ESCAPE])
	{
		DestroyWindow(mWindowHandle);
	}
	else if (mPressedKeys[VK_F1])
	{
		if (mRenderer == mRayTracer)
			mRenderer = mOpenGLRenderer;
		else
			mRenderer = mRayTracer;
		mRenderer->SetScene(mScene);
	}
	else if (mPressedKeys[VK_F2])
	{
		EnableRayDebugging();
	}
	else if (mPressedKeys[VK_F5])
	{
		mLoadScene = true;
	}
	else if (mPressedKeys[VK_NUMPAD4])
	{
		MoveDebugRayLeft(deltaTime);
	}
	else if (mPressedKeys[VK_NUMPAD6])
	{
		MoveDebugRayRight(deltaTime);
	}
	else if (mPressedKeys[VK_NUMPAD8])
	{
		MoveDebugRayUp(deltaTime);
	}
	else if (mPressedKeys[VK_NUMPAD5])
	{
		MoveDebugRayDown(deltaTime);
	}
	else if (mKeys[VK_LEFT] || mKeys[65])
	{
		MoveCameraLeft(deltaTime);
	}
	else if (mKeys[VK_RIGHT] || mKeys[68])
	{
		MoveCameraRight(deltaTime);
	}
	else if (mKeys[VK_UP] || mKeys[87])
	{
		MoveCameraForward(deltaTime);
	}
	else if (mKeys[VK_DOWN] || mKeys[83])
	{
		MoveCameraBackward(deltaTime);
	}
	else if (mKeys[81] || mKeys[33])
	{
		MoveCameraUp(deltaTime);
	}
	else if (mKeys[69] || mKeys[34])
	{
		MoveCameraDown(deltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayLeft(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	mDebugRayCoords.x() = srt_clamp(mDebugRayCoords.x() - deltaTime * DEBUG_RAY_MOVE_SPEED, 0, SCREEN_WIDTH - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayRight(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	mDebugRayCoords.x() = srt_clamp(mDebugRayCoords.x() + deltaTime * DEBUG_RAY_MOVE_SPEED, 0, SCREEN_WIDTH - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayUp(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	mDebugRayCoords.y() = srt_clamp(mDebugRayCoords.y() + deltaTime * DEBUG_RAY_MOVE_SPEED, 0, SCREEN_HEIGHT - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayDown(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}
	
	mDebugRayCoords.y() = srt_clamp(mDebugRayCoords.y() - deltaTime * DEBUG_RAY_MOVE_SPEED, 0, SCREEN_HEIGHT - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::UpdateDebugRay()
{
	assert(mRenderer == mOpenGLRenderer);
	auto rayIndex = static_cast<unsigned int>(mDebugRayCoords.y()) * SCREEN_WIDTH + static_cast<unsigned int>(mDebugRayCoords.x());
	// DEBUG:
	//std::cout << "rayIndex: " << rayIndex << std::endl;
	mOpenGLRenderer->SetDebugRay(mRayTracer->GetRaysMetadata()[rayIndex]);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::KeyDown(unsigned int virtualKey)
{
	mKeys[virtualKey] = true;
	mPressedKeys[virtualKey] = true;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::KeyUp(unsigned int virtualKey)
{
	mKeys[virtualKey] = false;
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
	if (IsRayTracingEnabled())
	{
		return;
	}

	if (!mRightMouseButtonPressed)
	{
		return;
	}

	if (mLastMousePosition.x() == x && mLastMousePosition.y() == y)
	{
		return;
	}

	Vector2F mousePosition = Vector2F((float)x, (float)y);

	if (mLastMousePosition.x() < 0 && mLastMousePosition.y() < 0)
	{
		mLastMousePosition = mousePosition;
		return;
	}

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
	mCameraTheta = srt_clamp(mCameraTheta + ANGLE_INCREMENT, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::TurnCameraDown()
{
	mCameraTheta = srt_clamp(mCameraTheta - ANGLE_INCREMENT, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::TurnCameraLeft()
{
	mCameraPhi -= ANGLE_INCREMENT /* NOTE: handiness sensitive */;
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::TurnCameraRight()
{
	mCameraPhi += ANGLE_INCREMENT /* NOTE: handiness sensitive */;
	UpdateCameraRotation();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::UpdateCameraRotation()
{
	float cosPhi = cos(mCameraPhi);
	float sinPhi = sin(mCameraPhi);
	float cosTheta = cos(mCameraTheta);
	float sinTheta = sin(mCameraTheta);
	Vector3F z(cosTheta * cosPhi, sinTheta, cosTheta * sinPhi);
	Vector3F y(-sinTheta * cosPhi, cosTheta, -sinTheta * sinPhi);
	Vector3F x = y.Cross(z);
	auto& camera = mScene->GetCamera();
	camera->localTransform.rotation = Matrix3x3F(x, y, z);
}

//////////////////////////////////////////////////////////////////////////
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
