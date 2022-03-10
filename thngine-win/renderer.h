#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dxgi1_2.h>

using namespace Microsoft::WRL;

namespace Thngine
{
	namespace Renderer
	{
		extern ComPtr<ID3D11Device> d3dDevice;
		extern ComPtr<ID3D11DeviceContext> d3dImmediateContext;
		extern ComPtr<IDXGISwapChain1> dxgiSwapChain;
		extern ComPtr<ID3D11Texture2D> d3dDepthStencilBuffer;
		extern ComPtr<ID3D11RenderTargetView> d3dRenderTargetView;
		extern ComPtr<ID3D11DepthStencilView> d3dDepthStencilView;
		extern ComPtr<ID3D11Texture2D> d3dBackBuffer;
		extern D3D11_VIEWPORT d3dViewport;

		extern ComPtr<IDXGIDevice1> dxgiDevice;
		extern ComPtr<IDXGIAdapter> dxgiAdapter;
		extern ComPtr<IDXGIFactory2> dxgiFactory;
		extern ComPtr<IDXGISurface> dxgiBackBuffer;

		extern ComPtr<ID2D1Factory1> d2dFactory;
		extern ComPtr<ID2D1Device> d2dDevice;
		extern ComPtr<ID2D1DeviceContext> d2dDeviceContext;
		extern ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

		HRESULT InitRenderer();
		void OnResize();

		void Predraw();
		void Render();

		void Deinit();

		void DrawRectangle(int x, int y, int width, int height, ID2D1Brush* brush);
		ID2D1SolidColorBrush* CreateSolidColorBrush(D2D1_COLOR_F color);
	}
}