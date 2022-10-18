#include "Device.h"
#include "Context.h"


Device::~Device()
{
}

HRESULT Device::CreateDevice(Context* c_context,Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context , Microsoft::WRL::ComPtr<IDXGIFactory1> device_factory)
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
            D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &device_context);

        if (hr == E_INVALIDARG)
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &device_context);
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
                hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(device_factory.GetAddressOf()));
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;

    c_context->SetFactory1(device_factory);
    c_context->SetDeviceContext(device_context);

    return S_OK;

}



/*Depth Works  We Set Render Target In Context*/
void Device::CreateDepth()
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


}

void Device::CreatePost()
{
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descTexture = {};
    descTexture.Width = _viewWidth;
    descTexture.Height = _viewHeight;
    descTexture.MipLevels = 1;
    descTexture.ArraySize = 1;
    descTexture.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    descTexture.SampleDesc.Count = 1;
    descTexture.Usage = D3D11_USAGE_DEFAULT;
    descTexture.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    descTexture.CPUAccessFlags = 0;
    descTexture.MiscFlags = 0;

    _pd3dDevice->CreateTexture2D(&descTexture, nullptr, &_pRTTRenderTargetTexture);
}



 void Device::CreateRTTRenderTargetView(Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain , ID3D11Texture2D* texture)
{
    // Create a render target view
    ID3D11Texture2D* pBackBuffer = texture;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, _pRTTRenderTargetView.GetAddressOf());
    pBackBuffer->Release();
    
}
 void Device::CreateRenderTargetView( Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain, ID3D11Texture2D* texture)
 {
     // Create a render target view
     ID3D11Texture2D* pBackBuffer = texture;
     swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
     _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, _pRenderTargetView.GetAddressOf());
     pBackBuffer->Release();

 }

//Old InitMesh Function
HRESULT	Device::CreateConstantBuffer()
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