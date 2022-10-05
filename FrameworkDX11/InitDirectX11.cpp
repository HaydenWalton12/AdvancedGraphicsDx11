#include "InitDirectX11.h"



//--------------------------------------------------------------------------------------
// Order of initialising Direct3D resources, required for runtime
//--------------------------------------------------------------------------------------
void InitDirectX11::Init()
{

    InitDevice();
    InitSwapChain();
    InitRenderTargetView();
    InitConstantBuffer();
    InitImGUi();
    InitDepth();
}

//--------------------------------------------------------------------------------------
// Create Direct3D device
//--------------------------------------------------------------------------------------

HRESULT InitDirectX11::InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(_Hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &_pImmediateContext);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        }

        if (SUCCEEDED(hr))
            break;
    }
    if (FAILED(hr))
        return hr;

    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)

    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = _pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(_pdxgiFactory1.GetAddressOf()));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    if (FAILED(hr))
        return hr;



    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);



    if (FAILED(hr))
        return hr;



    return S_OK;
}

//Old InitMesh Function
HRESULT		InitDirectX11::InitConstantBuffer()
{
    HRESULT hr;

    //May Or May Not need to hhange this , since we may only be using cubes for this simulation.
    // Create the constant buffer
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);
    if (FAILED(hr))
        return hr;



    // Create the light constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pLightConstantBuffer);
    if (FAILED(hr))
        return hr;


    return hr;
}

void InitDirectX11::InitRenderTargetView()
{
    
    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
     _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

}



void InitDirectX11::InitImGUi()
{
    //Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(_Hwnd);
    ImGui_ImplDX11_Init(_pd3dDevice.Get(), _pImmediateContext.Get());
    ImGui::StyleColorsDark();
}
void InitDirectX11::InitSampler()
{
}


void InitDirectX11::InitDepth()
{

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = _viewWidth;
    descDepth.Height = _viewHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
   _pd3dDevice->CreateTexture2D(&descDepth, nullptr, &_pDepthStencil);


    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    _pd3dDevice->CreateDepthStencilView(_pDepthStencil.Get(), &descDSV, _pDepthStencilView.GetAddressOf());


    _pImmediateContext.Get()->OMSetRenderTargets(1, _pRenderTargetView.GetAddressOf(), _pDepthStencilView.Get());
}
/*
IDXGIFactory - Interface for generating DXGI Objects , such as swap chain

*/
void InitDirectX11::InitSwapChain()
{
    HRESULT hr = S_OK;
    RECT rc;
    GetClientRect(_Hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)

    // Create swap chain

    hr = _pdxgiFactory1.Get()->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(_pdxgiFactory2.GetAddressOf()));
    if (_pdxgiFactory2.Get())
    {
        // DirectX 11.1 or later
        hr = _pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(_pd3dDevice1.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            (void)_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(_pImmediateContext1.GetAddressOf()));
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;//  DXGI_FORMAT_R16G16B16A16_FLOAT;////DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = _pdxgiFactory2->CreateSwapChainForHwnd(_pd3dDevice.Get(), _Hwnd, &sd, nullptr, nullptr, &_pSwapChain1);
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

        hr = _pdxgiFactory1->CreateSwapChain(_pd3dDevice.Get(), &sd, &_pSwapChain);
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    _pdxgiFactory1->MakeWindowAssociation(_Hwnd, DXGI_MWA_NO_ALT_ENTER);

    _pdxgiFactory1->Release();
}
