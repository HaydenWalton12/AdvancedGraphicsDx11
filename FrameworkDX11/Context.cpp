#include "Context.h"

Device* _pDevice = nullptr;

void Context::SetViewport(int width , int height)
{
    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pDeviceContext->RSSetViewports(1, &vp);
}

void Context::SetRenderTargetView(ID3D11RenderTargetView* render_target, ID3D11DepthStencilView* depth_view)
{
    _pDeviceContext.Get()->OMSetRenderTargets(1, &render_target, depth_view);
}

/*
IDXGIFactory - Interface for generating DXGI Objects , such as swap chain
*/
HRESULT Context::SetSwapChain(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
    HRESULT hr = S_OK;
    RECT rc;
    GetClientRect(_Hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)

    // Create swap chain
    Microsoft::WRL::ComPtr<ID3D11Device1> device_1;
    hr = _pdxgiFactory1.Get()->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(_pdxgiFactory2.GetAddressOf()));
    if (_pdxgiFactory2.Get())
    {
        // DirectX 11.1 or later
        hr = device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(device_1.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            (void)_pDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(_pDeviceContext1.GetAddressOf()));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;//  DXGI_FORMAT_R16G16B16A16_FLOAT;////DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = _pdxgiFactory2->CreateSwapChainForHwnd(device.Get(), _Hwnd, &sd, nullptr, nullptr, &_pSwapChain1);
        if (SUCCEEDED(hr))
        {
            hr = _pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(_pSwapChain.GetAddressOf()));
        }

        _pdxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = _Hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = _pdxgiFactory1->CreateSwapChain(device.Get(), &sd, &_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    _pdxgiFactory1->MakeWindowAssociation(_Hwnd, DXGI_MWA_NO_ALT_ENTER);

    _pdxgiFactory1->Release();
    return S_OK;
}