#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include <wrl.h>
class Device;
/*
	Class that represents ID3D11DeviceContext, which is used for rendering commands

*/
class Context
{
public:
	Context(HWND hwnd, int width, int height) : _Hwnd(hwnd), _viewWidth(width), _viewHeight(height)
	{

	}

	 Microsoft::WRL::ComPtr<ID3D11DeviceContext1> _pDeviceContext1;
	 Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pDeviceContext;
	void SetDeviceContext(Microsoft::WRL::ComPtr<ID3D11DeviceContext> device)
	 {
		_pDeviceContext = device;
	 }
	 Microsoft::WRL::ComPtr<ID3D11DeviceContext1> GetDeviceContext1() { return _pDeviceContext1; }
	 Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _pDeviceContext; }

	 Microsoft::WRL::ComPtr<IDXGIFactory1> _pdxgiFactory1;
	 Microsoft::WRL::ComPtr<IDXGIFactory2> _pdxgiFactory2;

	 void SetFactory1(Microsoft::WRL::ComPtr<IDXGIFactory1> factory)
	 {
		 _pdxgiFactory1 = factory;
	 }
	 Microsoft::WRL::ComPtr<IDXGIFactory1>GetFactory1() { return _pdxgiFactory1; }
	 Microsoft::WRL::ComPtr<IDXGIFactory2>GetFactory2() { return _pdxgiFactory2; }

	 Microsoft::WRL::ComPtr<IDXGISwapChain1> _pSwapChain1;
	 Microsoft::WRL::ComPtr<IDXGISwapChain>	_pSwapChain;
     Microsoft::WRL::ComPtr<IDXGISwapChain1> GetSwapChain1() { return _pSwapChain1; }
	 Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return _pSwapChain; }

	HWND _Hwnd;

	Device* _pDevice;
	HRESULT SetSwapChain(Microsoft::WRL::ComPtr<ID3D11Device> device);

	void SetViewport(int width, int height);
	void SetRenderTargetView(ID3D11RenderTargetView* render_target[], ID3D11DepthStencilView* depth_view, int num_views);

	int	 _viewWidth;
	int	_viewHeight;

};

