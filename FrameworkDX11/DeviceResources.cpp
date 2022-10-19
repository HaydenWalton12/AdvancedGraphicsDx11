#include "DeviceResources.h"

DeviceResources::DeviceResources()
{
}


DeviceResources::~DeviceResources()
{
}

//Configures the Direct3D device, sytores and handles to it
void DeviceResources::CreateDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG

    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif

   

    //Determines DirectX Hardware levels the app will support
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    
    UINT numFeatureLevels = ARRAYSIZE(featureLevels); 


    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;


    
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_NULL,
            nullptr,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            device.GetAddressOf(),  // Returns the Direct3D device created.
            &_featureLevel,     // Returns feature level of device created.
            context.GetAddressOf()  // Returns the device immediate context.
        );
    

  
    device.As(&_d3dDevice);
    context.As(&_d3dContext);
    IDXGIDevice* dxgiDevice = nullptr;
    hr = _d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (SUCCEEDED(hr))
    {
        IDXGIAdapter* adapter = nullptr;
        hr = dxgiDevice->GetAdapter(&adapter);
        if (SUCCEEDED(hr))
        {
            hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(_dxgiFactory1.GetAddressOf()));
            adapter->Release();
        }
        dxgiDevice->Release();
    }
}
//Resources need  to be recreated everytime window size is changed, further required for rendering tasks
void DeviceResources::CreateResources()
{
    //Clear Previous Resources window size context
    _d3dContext->OMSetRenderTargets(0, nullptr, nullptr);
    _d3dRenderTargetView.Reset();
    _d3dDepthStencilView.Reset();
    _renderTarget.Reset();
    _depthStencil.Reset();
    _d3dContext.Reset();

    //Determine Render target size

    //Cast - Forces One Data Type to be converted into another
    //static_cast - performs a nonpolymorphic cast.
    const UINT backBufferWidth = static_cast<UINT>(_windowWidth);
    const UINT backBufferHeight = static_cast<UINT>(_windowWidth);

    //If Swapchain already exists, resize to accomidate new window values
    if (_swapChain != nullptr)
    {

        HRESULT hr = _swapChain->ResizeBuffers
        (
            _backBufferCount,
            backBufferWidth,
            backBufferHeight,
            _backBufferFormat,
            0


        );
    }
    //Create SwapChain
    else
    {

        // Create swap chain
        Microsoft::WRL::ComPtr<ID3D11Device1> device_1;
        _dxgiFactory1.Get()->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(_dxgiFactory2.GetAddressOf()));
        if (_dxgiFactory2.Get())
        {
            // DirectX 11.1 or later
            _d3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(device_1.GetAddressOf()));
           
            DXGI_SWAP_CHAIN_DESC1 sd = {};
            sd.Width = backBufferWidth;
            sd.Height = backBufferHeight;
            sd.Format = _backBufferFormat;//  DXGI_FORMAT_R16G16B16A16_FLOAT;////DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            _dxgiFactory2->CreateSwapChainForHwnd(_d3dDevice.Get(), _window, &sd, nullptr, nullptr, &_swapChain1);
           

            _dxgiFactory2->Release();
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = backBufferWidth;
            sd.BufferDesc.Height = backBufferHeight;
            sd.BufferDesc.Format = _backBufferFormat;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = _window;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            _dxgiFactory1->CreateSwapChain(_d3dDevice.Get(), &sd, &_swapChain);
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        _dxgiFactory1->MakeWindowAssociation(_window, DXGI_MWA_NO_ALT_ENTER);

        _dxgiFactory1->Release();
  
    }

}
