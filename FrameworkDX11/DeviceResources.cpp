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
//Used to initially create resources & when resources are changed, they are remade to fit viewport
void DeviceResources::CreateResources()
{
    //Clear Previous Resources window size context
    _d3dContext->OMSetRenderTargets(0, nullptr, nullptr);
    _d3dRenderTargetView.Reset();
    _d3dDepthStencilView.Reset();
    _renderTarget.Reset();
    _depthStencil.Reset();
    _d3dContext->Flush();

    //Determine Render target size

    //Cast - Forces One Data Type to be converted into another
    //static_cast - performs a nonpolymorphic cast.
    const UINT backBufferWidth = static_cast<UINT>(_windowWidth);
    const UINT backBufferHeight = static_cast<UINT>(_windowHeight);

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
        

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        _dxgiFactory1->MakeWindowAssociation(_window, DXGI_MWA_NO_ALT_ENTER);

        _dxgiFactory1->Release();
  
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    //Create A Render Target view of the swap chain back buffer
    _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D) , &_renderTarget);
    _d3dDevice->CreateRenderTargetView(_renderTarget.Get(), nullptr, _d3dRenderTargetView.ReleaseAndGetAddressOf());

    //Create Depth Stencil
    //Allocates A 2D Surface
    // Create/Recreate depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = backBufferWidth;
    descDepth.Height = backBufferHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    _d3dDevice->CreateTexture2D(&descDepth, nullptr, _depthStencil.ReleaseAndGetAddressOf());
    
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    _d3dDevice->CreateDepthStencilView(_depthStencil.Get(), &descDSV, _d3dDepthStencilView.GetAddressOf());



}

void DeviceResources::SetWindow(HWND window, int height, int width)
{
    _window = window;
    GetClientRect(_window , &_outputSize);
    _windowWidth = _outputSize.right - _outputSize.left;
    _windowHeight = _outputSize.bottom - _outputSize.top;

}

void DeviceResources::WindowSizedChanged(int width, int height)
{


}

void DeviceResources::CreateConstantBuffer()
{
    HRESULT hr;

    //May Or May Not need to hhange this , since we may only be using cubes for this simulation.
    // Create the constant buffer
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Mesh) * 24;
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
    hr = _d3dDevice->CreateBuffer(&bd, nullptr, &_ConstantBuffer);




    // Create the light constant buffer
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(LightPropertiesConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = _d3dDevice->CreateBuffer(&bd, nullptr, &_LightConstantBuffer);



}