#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "Scene.h"
#include "Renderer.h"
#include "RayTracer.h"
#include "OpenGLRenderer.h"
#include "ColorRGBA.h"

#include <memory>
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"

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
	static const ColorRGBA GLOBAL_AMBIENT_LIGHT;

	inline static SimpleRayTracerApp* GetInstance()
	{
		return s_mpInstance;
	}

	int Run(unsigned int argc, const char** argv);

	void EnableRayDebugging(const Vector2F& rRayToDebug);
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
	Vector2F mLastMousePosition;
	float mCameraYaw;
	float mCameraPitch;

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
	void MoveCameraLeft();
	void MoveCameraRight();
	void MoveCameraForward();
	void MoveCameraBackward();
	void MoveCameraUp();
	void MoveCameraDown();
	void TurnCameraUp();
	void TurnCameraDown();
	void TurnCameraLeft();
	void TurnCameraRight();
	void UpdateCameraRotation();
	bool IsRayTracingEnabled() const;

};


#endif