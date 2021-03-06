#include <windowsx.h>
#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>

#include "Common.h"
#include "SimpleRayTracerApp.h"
#include "Vector3F.h"
#include "ColorRGBA.h"
#include "Sphere.h"
#include "Mesh.h"
#include "PointLight.h"
#include "TextureLoader.h"
#include "SceneLoader.h"
#include "StringUtils.h"

#define Win32Assert(resultHandle, errorMessage) \
	if ((resultHandle) == 0) \
	{ \
		MessageBox(NULL, #errorMessage, WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION); \
		Dispose(); \
		exit(EXIT_FAILURE); \
	} \

#define Assert(result, errorMessage) \
	if (!(result)) \
	{ \
		MessageBox(NULL, #errorMessage, WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION); \
		Dispose(); \
		exit(EXIT_FAILURE); \
	} \

SimpleRayTracerApp* SimpleRayTracerApp::s_mpInstance = 0;
const char* SimpleRayTracerApp::WINDOW_TITLE = "simpleraytracer";
const char* SimpleRayTracerApp::WINDOW_CLASS_NAME = "simpleraytracerWindowClass";
const unsigned int SimpleRayTracerApp::SCREEN_WIDTH = 640;
const unsigned int SimpleRayTracerApp::SCREEN_HEIGHT = 480;
const unsigned int SimpleRayTracerApp::BYTES_PER_PIXEL = 4;
const unsigned int SimpleRayTracerApp::COLOR_BUFFER_BITS = 32;
const unsigned int SimpleRayTracerApp::DEPTH_BUFFER_BITS = 32;
const unsigned int SimpleRayTracerApp::HAS_ALPHA = 0;
const PIXELFORMATDESCRIPTOR SimpleRayTracerApp::PIXEL_FORMAT_DESCRIPTOR = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, COLOR_BUFFER_BITS, 0, 0, 0, 0, 0, 0,	HAS_ALPHA, 0, 0, 0, 0, 0, 0, DEPTH_BUFFER_BITS, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
const ColorRGBA SimpleRayTracerApp::CLEAR_COLOR(0, 0, 0, 1);
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

	if (!mRayTracer->CollectRayMetadataEnabled())
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
int SimpleRayTracerApp::Run()
{
	std::string commandLine (GetCommandLine());

	std::vector<std::string> tokens;
	StringUtils::Tokenize(commandLine, " ", tokens);

	if (tokens.size() < 2)
	{
		MessageBox(0, "no scene file specified", WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		exit(EXIT_FAILURE);
	}

	mApplicationHandle = GetModuleHandle(0);

	Win32Assert(RegisterClassEx(&CreateWindowClass()), "RegisterClassEx failed");
	Win32Assert((mWindowHandle = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN), CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, mApplicationHandle, NULL)), "CreateWindow failed");
	Win32Assert((mDeviceContextHandle = GetDC(mWindowHandle)), "GetDC() failed");
	Win32Assert((mPixelFormat = ChoosePixelFormat(mDeviceContextHandle, &PIXEL_FORMAT_DESCRIPTOR)), "ChoosePixelFormat() failed");
	Win32Assert(SetPixelFormat(mDeviceContextHandle, mPixelFormat, &PIXEL_FORMAT_DESCRIPTOR), "SetPixelFormat() failed");
	Win32Assert((mOpenGLRenderingContextHandle = wglCreateContext(mDeviceContextHandle)), "wglCreateContext() failed");
	Win32Assert(wglMakeCurrent(mDeviceContextHandle, mOpenGLRenderingContextHandle), "wglMakeCurrent() failed");
	ShowWindow(mWindowHandle, SW_SHOW);
	SetForegroundWindow(mWindowHandle);
	SetFocus(mWindowHandle);

	mpSceneFileName = tokens[1];

	bool onlyOneFrame = false;
	if (tokens.size() >= 3)
	{
		onlyOneFrame = atoi(tokens[2].c_str()) != 0;
	}

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

		try
		{
			auto start = std::chrono::system_clock::now().time_since_epoch();
			if (mLoadScene)
			{
				LoadSceneFromXML();
				mRayTracer->SetScene(mScene);
				mOpenGLRenderer->SetScene(mScene);
				mLoadScene = false;
			}
			mScene->Update();
			mRenderer->Render();
			auto end = std::chrono::system_clock::now().time_since_epoch();
			auto deltaTime = (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0f);
			ProcessKeys(deltaTime);
			memset(mPressedKeys, 0, sizeof(mPressedKeys));
			std::stringstream stream;
			stream << std::fixed << std::setprecision(5) << WINDOW_TITLE << " @ fps: " << (1 / deltaTime);
			SetWindowText(mWindowHandle, stream.str().c_str());
			SwapBuffers(mDeviceContextHandle);

			if (onlyOneFrame)
			{
				mRunning = false;
			}
		}
		catch (std::runtime_error& rException)
		{
			MessageBox(mWindowHandle, rException.what(), WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION);
		}
	}

	DestroyWindow(mWindowHandle);

	Dispose();

	Assert(UnregisterClass(WINDOW_CLASS_NAME, mApplicationHandle), "UnregisterClass() failed");

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
		Win32Assert(wglMakeCurrent(NULL, NULL), "wglMakeCurrent() failed");
		Win32Assert(wglDeleteContext(mOpenGLRenderingContextHandle), "wglDeleteContext() failed")
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
	camera->localTransform.position -= camera->localTransform.right(); // *deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraRight(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.right(); // *deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraForward(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position -= camera->localTransform.forward(); // *deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraBackward(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.forward(); // *deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraUp(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position += camera->localTransform.up(); // *deltaTime * CAMERA_MOVE_SPEED;
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveCameraDown(float deltaTime)
{
	if (IsRayTracingEnabled())
	{
		return;
	}

	auto& camera = mScene->GetCamera();
	camera->localTransform.position -= camera->localTransform.up(); // *deltaTime * CAMERA_MOVE_SPEED;
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
	else if (mPressedKeys[VK_F3])
	{
		ToggleRayTraceDebug();
	}
	else if (mPressedKeys[VK_F4])
	{
		ToggleCollectRayMetadata();
	}
	else if (mPressedKeys[VK_F5])
	{
		mLoadScene = true;
	}
	if (mKeys[VK_NUMPAD4])
	{
		MoveDebugRayLeft(deltaTime);
	}
	if (mKeys[VK_NUMPAD6])
	{
		MoveDebugRayRight(deltaTime);
	}
	if (mKeys[VK_NUMPAD8])
	{
		MoveDebugRayUp(deltaTime);
	}
	if (mKeys[VK_NUMPAD5])
	{
		MoveDebugRayDown(deltaTime);
	}
	if (mKeys[VK_LEFT] || mKeys[65])
	{
		MoveCameraLeft(deltaTime);
	}
	if (mKeys[VK_RIGHT] || mKeys[68])
	{
		MoveCameraRight(deltaTime);
	}
	if (mKeys[VK_UP] || mKeys[87])
	{
		MoveCameraForward(deltaTime);
	}
	if (mKeys[VK_DOWN] || mKeys[83])
	{
		MoveCameraBackward(deltaTime);
	}
	if (mKeys[81] || mKeys[33])
	{
		MoveCameraUp(deltaTime);
	}
	if (mKeys[69] || mKeys[34])
	{
		MoveCameraDown(deltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayLeft(float deltaTime)
{
	mDebugRayCoords.x() = srt_clamp(mDebugRayCoords.x() - 1 /*deltaTime * DEBUG_RAY_MOVE_SPEED*/, 0, SCREEN_WIDTH - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayRight(float deltaTime)
{
	mDebugRayCoords.x() = srt_clamp(mDebugRayCoords.x() + 1 /*deltaTime * DEBUG_RAY_MOVE_SPEED*/, 0, SCREEN_WIDTH - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayUp(float deltaTime)
{
	mDebugRayCoords.y() = srt_clamp(mDebugRayCoords.y() + 1 /*deltaTime * DEBUG_RAY_MOVE_SPEED*/, 0, SCREEN_HEIGHT - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::MoveDebugRayDown(float deltaTime)
{
	mDebugRayCoords.y() = srt_clamp(mDebugRayCoords.y() - 1 /*deltaTime * DEBUG_RAY_MOVE_SPEED*/, 0, SCREEN_HEIGHT - 1);
	UpdateDebugRay();
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::UpdateDebugRay()
{
	auto rayIndex = static_cast<unsigned int>(mDebugRayCoords.y()) * SCREEN_WIDTH + static_cast<unsigned int>(mDebugRayCoords.x());
	// DEBUG:
	//std::cout << "rayIndex: " << rayIndex << std::endl;
	mOpenGLRenderer->SetDebugRay(mRayTracer->GetRaysMetadata()[rayIndex]);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::ToggleCollectRayMetadata()
{
	bool collectRayMetadata = !mRayTracer->CollectRayMetadataEnabled();
	std::cout << "Collect Ray Metadata: " << srt_boolStr(collectRayMetadata) << std::endl;
	mRayTracer->SetCollectRayMetadata(collectRayMetadata);
}

//////////////////////////////////////////////////////////////////////////
void SimpleRayTracerApp::ToggleRayTraceDebug()
{
	bool rayTracerDebug = !mRayTracer->DebugEnabled();
	std::cout << "Ray Tracer Debug: " << srt_boolStr(rayTracerDebug) << std::endl;
	mRayTracer->SetDebug(rayTracerDebug);
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
	// DEBUG:
	//std::cout << "x: " << x.ToString() << ", y: " << y.ToString() << ", z: " << z.ToString() << std::endl;
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
		SimpleRayTracerApp::s_mpInstance->KeyDown(static_cast<unsigned int>(wParam));
		break;
	case WM_KEYUP:
		SimpleRayTracerApp::s_mpInstance->KeyUp(static_cast<unsigned int>(wParam));
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
