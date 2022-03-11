#include "window.h"
#include "input.h"

namespace Thngine
{
	namespace Window
	{
		HWND MainWindowHandle;
		HINSTANCE MainWindowInstance;
		static WNDCLASS MainWindowClass = { 0 };
		static LPCTSTR MainWindowClassName = TEXT("THNGINE");
		static LPCTSTR MainWindowTitle = TEXT("THNGINE");

		LRESULT CALLBACK MainWindowCallback(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
		{
			switch (umsg)
			{
			case WM_KEYDOWN:
				Thngine::Input::SetKeyDown(wparam, true);
				break;
			case WM_KEYUP:
				Thngine::Input::SetKeyDown(wparam, false);
				break;
			case WM_CLOSE:
				DestroyWindow(hwnd);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			}
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}

		HRESULT Init()
		{
			HRESULT result;

			MainWindowClass.lpszClassName = MainWindowClassName;
			MainWindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			MainWindowClass.hCursor = LoadIcon(MainWindowInstance, IDC_ARROW);
			MainWindowClass.hIcon = LoadIcon(MainWindowInstance, IDI_WINLOGO);
			MainWindowClass.hInstance = MainWindowInstance;
			MainWindowClass.lpszClassName = MainWindowClassName;
			MainWindowClass.lpfnWndProc = MainWindowCallback;
			MainWindowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

			if (!RegisterClass(&MainWindowClass))
				return ERROR_CLASS_DOES_NOT_EXIST;

			if (!(MainWindowHandle = CreateWindow(MainWindowClassName, MainWindowTitle, WS_VISIBLE|WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, MainWindowInstance, NULL)))
				return ERROR_INVALID_WINDOW_HANDLE;

			ShowWindow(MainWindowHandle, SW_SHOW);

			return ERROR_SUCCESS;
		}

		int GetMainWindowWidth()
		{
			RECT rect;
			GetWindowRect(Window::MainWindowHandle, &rect);
			return rect.right - rect.left;
		}

		int GetMainWindowHeight()
		{
			RECT rect;
			GetWindowRect(Window::MainWindowHandle, &rect);
			return rect.bottom - rect.top;
		}
	}
}