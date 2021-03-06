#pragma once

#include <Windows.h>

namespace Thngine
{
	namespace Window
	{
		extern HWND MainWindowHandle;
		extern HINSTANCE MainWindowInstance;

		LRESULT CALLBACK MainWindowCallback(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
		HRESULT Init();
		int GetMainWindowWidth();
		int GetMainWindowHeight();
	}
}