#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"

#include "Scene.h"
#include "Renderer.h"
#include "RayTracer.h"
#include "OpenGLRenderer.h"
#include "ColorRGBA.h"

//////////////////////////////////////////////////////////////////////////
int main(unsigned int argc, const char** argv);

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
class SimpleRayTracerApp
{
public:
	static const unsigned int SCREEN_WIDTH;
	static const unsigned int SCREEN_HEIGHT;
	static const unsigned int BYTES_PER_PIXEL;
	static const ColorRGBA CLEAR_COLOR;

	inline static SimpleRayTracerApp* GetInstance()
	{
		return s_mpInstance;
	}

	int Run(unsigned int argc, const char** argv);

	void EnableRayDebugging();
	void DisableRayDebugging();
	void Close();

	friend int main(unsigned int argc, const char** argv);
	friend LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static const char* WINDOW_TITLE;
	static const char* WINDOW_CLASS_NAME;
	static const unsigned int COLOR_BUFFER_BITS;
	static const unsigned int DEPTH_BUFFER_BITS;
	static const unsigned int HAS_ALPHA;
	static const PIXELFORMATDESCRIPTOR PIXEL_FORMAT_DESCRIPTOR;
	static const float ANGLE_INCREMENT;
	static const float CAMERA_PITCH_LIMIT;
	static const float CAMERA_MOVE_SPEED;
	static const float DEBUG_RAY_MOVE_SPEED;

	static SimpleRayTracerApp* s_mpInstance;

	bool mRunning;
	HINSTANCE mApplicationHandle;
	HWND mWindowHandle;
	HDC mDeviceContextHandle;
	int mPixelFormat;
	HGLRC mOpenGLRenderingContextHandle;
	const char* mpSceneFileName;
	std::shared_ptr<Scene> mScene;
	std::shared_ptr<RayTracer> mRayTracer;
	std::shared_ptr<OpenGLRenderer> mOpenGLRenderer;
	std::shared_ptr<Renderer> mRenderer;
	bool mLoadScene;
	bool mRightMouseButtonPressed;
	bool mKeys[0xFF];
	bool mPressedKeys[0xFF];
	Vector2F mLastMousePosition;
	float mCameraPhi;
	float mCameraTheta;
	Vector2F mDebugRayCoords;

	SimpleRayTracerApp();
	~SimpleRayTracerApp();

	void LoadSceneFromXML();
	void Dispose();
	WNDCLASSEX CreateWindowClass();
	void KeyDown(unsigned int virtualKey);
	void KeyUp(unsigned int virtualKey);
	void MouseButtonDown(unsigned int button, int x, int y);
	void MouseButtonUp(unsigned int button, int x, int y);
	void MouseMove(int x, int y);
	void MoveCameraLeft(float deltaTime);
	void MoveCameraRight(float deltaTime);
	void MoveCameraForward(float deltaTime);
	void MoveCameraBackward(float deltaTime);
	void MoveCameraUp(float deltaTime);
	void MoveCameraDown(float deltaTime);
	void TurnCameraUp();
	void TurnCameraDown();
	void TurnCameraLeft();
	void TurnCameraRight();
	void UpdateCameraRotation();
	bool IsRayTracingEnabled() const;
	void ProcessKeys(float deltaTime);
	void MoveDebugRayLeft(float deltaTime);
	void MoveDebugRayRight(float deltaTime);
	void MoveDebugRayUp(float deltaTime);
	void MoveDebugRayDown(float deltaTime);
	void UpdateDebugRay();
	void ToggleCollectRayMetadata();
	void ToggleRayTraceDebug();

};


#endif