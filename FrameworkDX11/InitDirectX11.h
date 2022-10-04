#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>

#include <windows.h>
#include <windowsx.h>

#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>

#include "DrawableGameObject.h"
#include "structures.h"

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

class InitDirectX11
{
public:

	InitDirectX11(HWND hwnd) : _Hwnd(hwnd)
	{

	}
	~InitDirectX11()
	{

	}
	void Init(HWND hwnd, int widht, int height);
	HRESULT InitDevice();
	HRESULT InitConstantBuffer();
	void InitImGUi();

	//DX11 Properties
	D3D_DRIVER_TYPE   _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_0;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> _pSwapChain1;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _pDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> _pImmediateContext1;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11Device1> _pd3dDevice1;
	Microsoft::WRL::ComPtr<ID3D11Device> _pd3dDevice	Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;


	
	Microsoft::WRL::ComPtr<IDXGISwapChain1> GetSwapChain1() { return _pSwapChain1; }
	Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return _pSwapChain; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> GetDeviceContext1() { return _pImmediateContext1; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _pImmediateContext; }
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;
	Camera* g_Camera = nullptr;


	void InitSampler();
	void InitDepth();
	void InitViewPort();
	void InitSwapChain();

	int						_viewWidth;
	int						_viewHeight;


	HWND _Hwnd;

};

