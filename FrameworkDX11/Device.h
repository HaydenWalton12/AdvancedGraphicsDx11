#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "structures.h"
#include <wrl.h>

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
};

class Context;
/*
	Represents Virtual Adapter , used to create resources
*/
class Device
{
public:
	Device(HWND hwnd, int width, int height) : _Hwnd(hwnd), _viewWidth(width), _viewHeight(height)
	{
		
	}
	~Device();
	HWND _Hwnd;

	//DX11 Properties
	D3D_DRIVER_TYPE   _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_0;


	    Microsoft::WRL::ComPtr<ID3D11Device> _pd3dDevice;
	    Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return _pd3dDevice; }


	
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>GetRenderTargetView() { return  _pRenderTargetView; }

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _pDepthStencil;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>GetDepthStencilView() { return _pDepthStencilView; }
		Microsoft::WRL::ComPtr<ID3D11Texture2D>GetDepthStencil() { return _pDepthStencil; }



		Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _pLightConstantBuffer;

		Microsoft::WRL::ComPtr<ID3D11Buffer>GetConstantBuffer() { return _pConstantBuffer; }
		Microsoft::WRL::ComPtr<ID3D11Buffer>GetLightConstantBuffer() { return _pLightConstantBuffer; }

	void CreateDepth();
	HRESULT CreateDevice(Context* c_context, Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context, Microsoft::WRL::ComPtr<IDXGIFactory1> device_factory);
	void CreateRenderTargetView(Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain);

	HRESULT CreateConstantBuffer();

	
	int						_viewWidth;
	int						_viewHeight;

};

