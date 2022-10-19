#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "structures.h"
#include <wrl.h>


struct Mesh
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	//Additional Values For Bump Mapping Technique Normal Mapping
	XMFLOAT3 Tangent;
	XMFLOAT3 BiTangent;
};

/*

	DeviceResources.h - Wrapper Class For Direct3D 11 Device & SwapChain

	Wrapper - A class which is utility is which manages the resources of a class

	Class is inspired From DirectTK Samples

*/
class DeviceResources
{
public:
	DeviceResources();
	~DeviceResources();
	 
	//Creates Device Resources
	void CreateDevice();	//|
	void CreateResources();	//|Split  functions into two for simplicity purposes
	void SetWindow(HWND window, int height, int width);
	void WindowSizedChanged(int height, int width);


	// Direct3D Accessors.
	auto                    GetD3DDevice() const noexcept { return _d3dDevice.Get(); }
	auto                    GetD3DDeviceContext() const noexcept { return _d3dContext.Get(); }
	auto                    GetSwapChain() const noexcept { return _swapChain.Get(); }
	//auto                    GetDXGIFactory() const noexcept { return _dxgiFactory1.Get(); }
	HWND                    GetWindow() const noexcept { return _window; }
	D3D_FEATURE_LEVEL       GetDeviceFeatureLevel() const noexcept { return _d3dFeatureLevel; }
	ID3D11Texture2D* GetRenderTarget() const noexcept { return _renderTarget.Get(); }
	ID3D11Texture2D* GetDepthStencil() const noexcept { return _depthStencil.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() const noexcept { return _d3dRenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const noexcept { return _d3dDepthStencilView.Get(); }




private:
	//Direct3D Objects - Used For Utility/ Initialisation purposes

	Microsoft::WRL::ComPtr<IDXGIFactory1>               _dxgiFactory1;
	Microsoft::WRL::ComPtr<IDXGIFactory2>               _dxgiFactory2;
	Microsoft::WRL::ComPtr<ID3D11Device1>              _d3dDevice; //Represents Virtual Adapter , used to create resources
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1>       _d3dContext; //Class that represents ID3D11DeviceContext, which is used for rendering commands
	Microsoft::WRL::ComPtr<IDXGISwapChain>            _swapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>            _swapChain1;



	//Direct3D Rendering Objects - Straight Forward
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         _renderTarget;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>         _depthStencil;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  _d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  _d3dDepthStencilView;



	//Direct 3D Properties 
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_FORMAT       _backBufferFormat  = DXGI_FORMAT_B8G8R8A8_UNORM; // For SwapChain , Given Format
	DXGI_FORMAT        _depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT				_backBufferCount = 2;
	//Cached Device Properties
	HWND                                            _window;
	D3D_FEATURE_LEVEL                               _d3dFeatureLevel;
	RECT                                            _outputSize;
	int												_windowWidth;
	int												_windowHeight;
		




};

