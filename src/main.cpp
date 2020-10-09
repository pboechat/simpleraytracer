#include <windows.h>

#include "SimpleRayTracerApp.h"

//////////////////////////////////////////////////////////////////////////
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SimpleRayTracerApp application;
	return application.Run();
}