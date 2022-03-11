#include "renderer.h"
#include "window.h"
#include <map>

namespace Thngine
{
	namespace Renderer
	{
		ComPtr<ID3D11Device> d3dDevice;
		ComPtr<ID3D11DeviceContext> d3dImmediateContext;
		ComPtr<IDXGISwapChain1> dxgiSwapChain;
		ComPtr<ID3D11Texture2D> d3dDepthStencilBuffer;
		ComPtr<ID3D11RenderTargetView> d3dRenderTargetView;
		ComPtr<ID3D11DepthStencilView> d3dDepthStencilView;
		ComPtr<ID3D11Texture2D> d3dBackBuffer;
		D3D11_VIEWPORT d3dViewport;

		ComPtr<IDXGIDevice1> dxgiDevice;
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ComPtr<IDXGIFactory2> dxgiFactory;
		ComPtr<IDXGISurface> dxgiBackBuffer;

		ComPtr<ID2D1Factory1> d2dFactory;
		ComPtr<ID2D1Device> d2dDevice;
		ComPtr<ID2D1DeviceContext> d2dDeviceContext;
		ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

		ComPtr<IDWriteFactory> dwriteFactory;
		ComPtr<IWICImagingFactory> wicImagingFactory;

		static HRESULT Resize();
		static HRESULT InitDirect3D();
		static HRESULT InitDirect2D();

		void Predraw()
		{
			d2dDeviceContext->BeginDraw();

			static float clearColor[] = {
				1.0f,
				1.0f,
				1.0f,
				1.0f
			};

			d3dImmediateContext->ClearRenderTargetView(d3dRenderTargetView.Get(), clearColor);
			d3dImmediateContext->ClearDepthStencilView(d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}

		void Render()
		{
			d2dDeviceContext->EndDraw();
			dxgiSwapChain->Present(0, 0);
		}

		void Deinit()
		{
			if (d3dImmediateContext)
				d3dImmediateContext->ClearState();
		}

		HRESULT Init()
		{
			HRESULT result;

			if (FAILED((result = InitDirect3D())))
				return result;

			if (FAILED((result = InitDirect2D())))
				return result;

			Resize();

			return ERROR_SUCCESS;
		}

		void OnResize()
		{
			HRESULT result;

			d3dRenderTargetView.Reset();
			d3dDepthStencilView.Reset();
			d3dDepthStencilBuffer.Reset();

			ComPtr<ID3D11Texture2D> backBuffer;

			if (FAILED(result = dxgiSwapChain->ResizeBuffers(
				1,
				Window::GetMainWindowWidth(),
				Window::GetMainWindowHeight(),
				DXGI_FORMAT_R8G8B8A8_UNORM,
				0)))
				return;

			if (FAILED(result = dxgiSwapChain->GetBuffer(
				0,
				__uuidof(ID3D11Texture2D),
				(void**)backBuffer.GetAddressOf())))
				return;
			if (FAILED(result = d3dDevice->CreateRenderTargetView(
				backBuffer.Get(),
				NULL,
				d3dRenderTargetView.GetAddressOf()
			)))
				return;

			backBuffer.Reset();

			Resize();
		}

		static HRESULT Resize()
		{
			HRESULT result;

			// Initialize depth stencil descriptor.
			D3D11_TEXTURE2D_DESC depthStencilDesc;

			depthStencilDesc.Width = Window::GetMainWindowWidth();
			depthStencilDesc.Height = Window::GetMainWindowHeight();

			depthStencilDesc.MipLevels = 1;

			depthStencilDesc.ArraySize = 1;

			depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;

			depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilDesc.CPUAccessFlags = 0;
			depthStencilDesc.MiscFlags = 0;

			// Create Depth Stencil View.
			if (FAILED(result = d3dDevice->CreateTexture2D(
				&depthStencilDesc,
				NULL,
				d3dDepthStencilBuffer.GetAddressOf())))
				return result;

			if (FAILED(result = d3dDevice->CreateDepthStencilView(
				d3dDepthStencilBuffer.Get(),
				NULL,
				d3dDepthStencilView.GetAddressOf())))
				return result;

			d3dImmediateContext->OMSetRenderTargets(
				1,
				d3dRenderTargetView.GetAddressOf(),
				d3dDepthStencilView.Get());

			d3dViewport.TopLeftX = 0;
			d3dViewport.TopLeftY = 0;
			d3dViewport.Width = (float)Window::GetMainWindowWidth();
			d3dViewport.Height = (float)Window::GetMainWindowHeight();
			d3dViewport.MinDepth = 0.0f;
			d3dViewport.MaxDepth = 1.0f;

			d3dImmediateContext->RSSetViewports(1, &d3dViewport);

			return ERROR_SUCCESS;
		}

		static HRESULT InitDirect3D()
		{
			HRESULT result;

			// Create D3D Device.
			D3D_DRIVER_TYPE driverTypes[] = {
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE,
				D3D_DRIVER_TYPE_SOFTWARE
			};

			D3D_FEATURE_LEVEL featureLevels[] = {
				D3D_FEATURE_LEVEL_11_0
			};

			D3D_FEATURE_LEVEL featureLevel;

			for (UINT i = 0; i < ARRAYSIZE(driverTypes); i++)
			{
				if (SUCCEEDED(result = D3D11CreateDevice(
					NULL,
					driverTypes[i],
					NULL,
					D3D10_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
					featureLevels,
					ARRAYSIZE(featureLevels),
					D3D11_SDK_VERSION,
					d3dDevice.GetAddressOf(),
					&featureLevel,
					d3dImmediateContext.GetAddressOf()
				)))
					break;
			}

			if (FAILED(result))
				return result;

			// Get DXGI Device.
			if (FAILED(result = d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice)))
				return result;

			// Get DXGI Adapter.
			if (FAILED(result = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf())))
				return result;

			// Get DXGI Factory.
			if (FAILED(result = dxgiAdapter->GetParent(
				__uuidof(IDXGIFactory1),
				(void**)dxgiFactory.GetAddressOf())))
				return result;

			// Initialize swap chain descriptor.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
			memset(&swapChainDesc, 0, sizeof(swapChainDesc));

			swapChainDesc.Width = Window::GetMainWindowWidth();
			swapChainDesc.Height = Window::GetMainWindowHeight();

			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;

			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;

			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

			swapChainDesc.Stereo = false;

			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;

			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			swapChainDesc.Flags = 0;

			if (FAILED(result = dxgiFactory->CreateSwapChainForHwnd(
				d3dDevice.Get(),
				Thngine::Window::MainWindowHandle,
				&swapChainDesc,
				NULL,
				NULL,
				dxgiSwapChain.GetAddressOf())))
				return result;

			// Disable ALT + Enter association.
			dxgiFactory->MakeWindowAssociation(
				Window::MainWindowHandle,
				DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

			if (FAILED(result = dxgiSwapChain->GetBuffer(
				0,
				__uuidof(ID3D11Texture2D),
				(void**)d3dBackBuffer.GetAddressOf())))
				return result;

			if (FAILED(result = d3dDevice->CreateRenderTargetView(
				d3dBackBuffer.Get(),
				NULL,
				d3dRenderTargetView.GetAddressOf())))
				return result;
			return ERROR_SUCCESS;
		}

		HRESULT InitDirect2D()
		{
			HRESULT result;

			dxgiDevice->SetMaximumFrameLatency(1);

			if (FAILED(result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), (void**)&d2dFactory)))
				return result;

			if (FAILED(result = d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf())))
				return result;

			if (FAILED(result = d2dDevice->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				d2dDeviceContext.GetAddressOf())))
				return result;

			if (FAILED(result = dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))))
				return result;

			if (FAILED(result = d2dDeviceContext->CreateBitmapFromDxgiSurface(
				dxgiBackBuffer.Get(),
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
				),
				d2dTargetBitmap.GetAddressOf())))
				return result;

			d2dDeviceContext->SetTarget(d2dTargetBitmap.Get());
			d2dDeviceContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
			d2dDeviceContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

			if (FAILED((result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),&dwriteFactory))))
				return result;

			if (FAILED((result =CoCreateInstance(CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,__uuidof(IWICImagingFactory),&wicImagingFactory))))
				return result;

			return ERROR_SUCCESS;
		}
	}
}