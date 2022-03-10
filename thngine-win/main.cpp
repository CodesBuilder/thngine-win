#include "task.h"
#include "window.h"
#include "renderer.h"
#include <Windows.h>

void MainThreadCallback()
{
	ComPtr<ID2D1SolidColorBrush> brush;
	brush = Thngine::Renderer::CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red));
	while (true)
	{
		Thngine::Renderer::DrawRectangle(300, 300, 100, 100, brush.Get());
		Thngine::Task::Wait(1);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT result;
	if (FAILED(result = CoInitialize(NULL)))
	{
		MessageBox(NULL, L"Unable to initialize COM", L"Error", MB_ICONERROR);
		return result;
	}

	Thngine::Window::MainWindowInstance = hInstance;
	
	if (FAILED(result=Thngine::Window::InitMainWindow()))
	{
		MessageBox(NULL, L"Unable to initialize main window", L"Error", MB_ICONERROR);
		return result;
	}

	if (FAILED(result=Thngine::Renderer::InitRenderer()))
	{
		MessageBox(NULL, L"Unable to initialize renderer", L"Error", MB_ICONERROR);
		return result;
	}

	Thngine::Task::CreateTask(MainThreadCallback);

	MSG msg;

	while (Thngine::Task::GetTaskCount())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		Thngine::Renderer::Predraw();

		Thngine::Task::Tick();

		Thngine::Renderer::Render();
	}

	Thngine::Renderer::Deinit();

	return 0;
}