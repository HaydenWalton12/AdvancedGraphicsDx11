#include "Home.h"




void Home::InitialiseApplication(HWND hwnd , HINSTANCE instance, int width , int height)
{

    _Instance = instance;

    _pDevice = new Device(hwnd, width, height);
    _pContext = new Context(hwnd, width, height);
    _pDevice->CreateDevice(_pContext , _pContext->GetDeviceContext().Get() , _pContext->GetFactory1().Get());
    _pContext->SetViewport(1280, 720);
    _pContext->SetSwapChain(_pDevice->GetDevice().Get());
    _pDevice->CreateRenderTargetView(_pContext->GetSwapChain().Get());

    _pDevice->CreateConstantBuffer();
    //Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get());
    ImGui::StyleColorsDark();

    _pDevice->CreateDepth();




    InitDirectInput(instance);
    InitScene(width, height);


}

//--------------------------------------------------------------------------------------
// Starts Scene Objects
//--------------------------------------------------------------------------------------
HRESULT Home::InitScene(int width, int height)
{
    HRESULT Hr;
        //Initialise Camera
    int g_viewWidth = 720;
    int g_viewHeight = 1280;
    _pCamera = new Camera(XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f), XMFLOAT4(0.0f, -0.05f, 0.05f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), g_viewWidth, g_viewHeight, XM_PIDIV2, 0.01f, 100.0f);
    
    _pObjectCube = new ObjectCube();
    _pObjectCube->InitMesh(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get());
    _pObjectCube->InitialiseShader(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get(), L"PixelShader.hlsl", L"VertexShader.hlsl");

    SimpleVertex v[] =
    {

        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

    };

    WORD indices[] = {

        // Front Facecolor
        0, 1, 2,
        0, 2, 3,

    };


    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = &v[0];
    HRESULT hr = _pDevice->GetDevice()->CreateBuffer(&bd, &InitData, _QuadVB.GetAddressOf());
    if (FAILED(hr))
        return hr;

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = &indices[0];
    hr = _pDevice->GetDevice()->CreateBuffer(&bd, &InitData, _QuadIB.GetAddressOf());
    if (FAILED(hr))
        return hr;


   _QuadShader = std::make_unique<Shader>(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get(), L"VertexShaderQuad.hlsl" , L"PixelShaderQuad.hlsl");

   bd.ByteWidth = sizeof(BlurBuffer);
   bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
   bd.CPUAccessFlags = 0;
   hr = _pDevice->GetDevice()->CreateBuffer(&bd, nullptr, &_pBlurBuffer );
   if (FAILED(hr))
       return hr;

   hr = CreateDDSTextureFromFile(_pDevice->GetDevice().Get(), L"Resources\\metal_grate_rusty_diff_4k.dds", nullptr,_pTextureResourceView.GetAddressOf());
   if (FAILED(hr))
       return hr;

   // Create depth stencil texture
   D3D11_TEXTURE2D_DESC textureDesc = {};
   textureDesc.Width = 1280;
   textureDesc.Height = 720;
   textureDesc.MipLevels = 1;
   textureDesc.ArraySize = 1;
   textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
   textureDesc.SampleDesc.Count = 1;
   textureDesc.Usage = D3D11_USAGE_DEFAULT;

   textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
   textureDesc.CPUAccessFlags = 0;
   textureDesc.MiscFlags = 0;

    _pDevice->GetDevice()->CreateTexture2D(&textureDesc, nullptr, _pRTTexture1.GetAddressOf());

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = _pRTTexture1.Get();
    _pContext->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    _pDevice->GetDevice()->CreateRenderTargetView(_pRTTexture1.Get(), nullptr, _pRTTRenderTargetView.GetAddressOf());
    pBackBuffer->Release();

    _pDevice->GetDevice()->CreateShaderResourceView(_pRTTexture1.Get() , nullptr , pRTTShaderResourceView.GetAddressOf());
    return S_OK;
    

}

void Home::Render()
{
   /*
    Post Processing Applied, it works by doing the following:
   
   Set render target to our RRTRenderView, this allows DirectX to know what render target we want our current render data to draw to , with the given correct properties
   todo so. In my case, a texture resource that will be the target to store this data, this data will be stored on the target view too (well has reference).

   We render entire scene to a to a texture (essentially a screen capture of our pre-rendered scene, since it hasnt been displayed to output merger yet)
   
   I then switch the render target view to my default target view(i say default since prior this was the only/defacto render view used)
   On this render view, I draw a fullscreen quad, this quad has a shader resource view(to process data via pixel / vertex shader stages) , this resource view
   stores the texture to our pre-rendered scene. We then process the texels/pixels, we can do this in a manor any way we want to , we then use this as the resultant
   */

    //Im GUI Draw Order , Must Be after dx11 draw order

    float t = CalculateDeltaTime(); // capped at 60 fps
    if (t == 0.0f)
        return;
    Input(_Instance);

    ClearRenderTarget();

    //Set Initial Render Target, This will be our Render Texture Target view
    // Draws our scene to a texture, we then draw this to the normal render target just as a texture, allowing us to use the quad
    _pContext->GetDeviceContext().Get()->OMSetRenderTargets(1, _pRTTRenderTargetView.GetAddressOf(), _pDevice->GetDepthStencilView().Get());


    // Update the cube transform, material etc. 
    _pObjectCube->Update(_pDevice, _pContext->GetDeviceContext().Get());

    UpdateConstantBuffer();



    Draw();

    //Set To Normal Render Target
    _pContext->GetDeviceContext().Get()->OMSetRenderTargets(1, _pDevice->GetRenderTargetView().GetAddressOf(), _pDevice->GetDepthStencilView().Get());
    _pContext->GetDeviceContext()->ClearDepthStencilView(_pDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pContext->GetDeviceContext()->IASetVertexBuffers(0, 1, _QuadVB.GetAddressOf(), &stride, &offset);

    _pContext->GetDeviceContext()->IASetIndexBuffer(_QuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);
    _pContext->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _pContext->GetDeviceContext()->IASetInputLayout(_QuadShader->GetVertexLayout().Get());

    //Tests wether the quad is physically drawn
   //HRESULT hr =  CreateDDSTextureFromFile(_pDevice->GetDevice().Get(), L"Resources\\color.dds", nullptr, pRTTShaderResourceView.GetAddressOf());
    //
    BlurBuffer blur;

    blur.horizontal = 1;
    blur.verticle = 1;
    blur.enable = 1;
    blur.padding;
    _pContext->GetDeviceContext()->UpdateSubresource(_pBlurBuffer.Get(), 0, nullptr, &blur, 0, 0);
    _pContext->GetDeviceContext()->PSSetConstantBuffers(0, 1, _pBlurBuffer.GetAddressOf());

    _pContext->GetDeviceContext()->VSSetShader(_QuadShader.get()->GetVertexShader().Get(), nullptr, 0);
    _pContext->GetDeviceContext()->PSSetShader(_QuadShader->GetPixelShader().Get(), nullptr, 0);

    //Quads shader resource view has stores our the texture we rendered previously , within quad shader we return the texture , however with final colour processing.
    _pContext->GetDeviceContext()->PSSetShaderResources(0, 1, pRTTShaderResourceView.GetAddressOf());
   // _pContext->GetDeviceContext()->PSSetShaderResources(1, 1, _pTextureResourceView.GetAddressOf());

    _pContext->GetDeviceContext()->DrawIndexed(6, 0, 0);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Engine Simulations");
    _pCamera->ImGuiCameraSettings();
    
    _pObjectCube->_ObjectProperties->_pShader.get()->ImGuiShaderSettings(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get());

    if (ImGui::CollapsingHeader("Active Lighting Controls"))
    {
        ImGui::DragFloat("X", &_Lighting.Position.x ,0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Y", &_Lighting.Position.y, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Z", &_Lighting.Position.z, 0.1f, -20.0f, 20.0f);
    }
    if (ImGui::CollapsingHeader("Active Cube Controls"))
    {
        ImGui::DragFloat("X", &_pObjectCube->_ObjectProperties->_Transformation.Translation.x, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Y", &_pObjectCube->_ObjectProperties->_Transformation.Translation.y, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Z", &_pObjectCube->_ObjectProperties->_Transformation.Translation.z, 0.1f, -20.0f, 20.0f);
    }
    if (ImGui::CollapsingHeader("Active Cube Rotate"))
    {
        ImGui::DragFloat("X", &_pObjectCube->_ObjectProperties->_Transformation.Rotation.x, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Y", &_pObjectCube->_ObjectProperties->_Transformation.Rotation.y, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Z", &_pObjectCube->_ObjectProperties->_Transformation.Rotation.z, 0.1f, -20.0f, 20.0f);
    }

    ImGui::End();
    //Render IMGUI
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    // Present our back buffer to our front buffer
    _pContext->GetSwapChain()->Present(0, 0);

}


float Home::CalculateDeltaTime()
{
    // Update our time
    static float deltaTime = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
    deltaTime = (timeCur - timeStart) / 1000.0f;
    timeStart = timeCur;

    float FPS60 = 1.0f / 60.0f;
    static float cummulativeTime = 0;

    // cap the framerate at 60 fps 
    cummulativeTime += deltaTime;
    if (cummulativeTime >= FPS60) {
        cummulativeTime = cummulativeTime - FPS60;
    }
    else {
        return 0;
    }
    return deltaTime;

}

void Home::Input(HINSTANCE instance)
{
    DIMOUSESTATE mouseCurrState;

    BYTE keyboardState[256];

    DIKeyBoard->Acquire();
    DIMouse->Acquire();

    DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
    DIKeyBoard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

    float xpos = _pObjectCube->_ObjectProperties->_Transformation.GetTranslate().x;
    float ypos = _pObjectCube->_ObjectProperties->_Transformation.GetTranslate().y;
    float zpos = _pObjectCube->_ObjectProperties->_Transformation.GetTranslate().z;

    //Forward
    if (keyboardState[DIK_W] & 0x80)
    {
        zpos += 0.001 / 1000;
    }

    //Backwards
    if (keyboardState[DIK_S] & 0x80)
    {
        zpos -= 0.001 / 1000;
    }

    //Right
    if (keyboardState[DIK_D] & 0x80)
    {
        xpos -= 0.001 / 1000;
    }

    //Left
    if (keyboardState[DIK_A] & 0x80)
    {
        xpos += 0.001 / 1000;
    }

    //Up
    if (keyboardState[DIK_SPACE] & 0x80)
    {
        ypos += 0.001 / 1000;
    }

    //Down
    if (keyboardState[DIK_LSHIFT] & 0x80)
    {
        ypos -= 0.001 / 1000;
    }




    //Update 
    _pObjectCube->_ObjectProperties->_Transformation.SetTranslation(XMFLOAT3(xpos, ypos, zpos));

}
void Home::InitDirectInput(HINSTANCE instance)
{
    DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
    //Create Keyboard Device
    DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyBoard, NULL);

    //Create Mouse Device
    DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

    //Input format of device
    DIKeyBoard->SetDataFormat(&c_dfDIKeyboard);
    DIKeyBoard->SetCooperativeLevel(NULL, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    //Input format of mouse device
    DIMouse->SetDataFormat(&c_dfDIMouse);
    DIMouse->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
}
void Home::ClearRenderTarget()
{
    // Clear the back buffer
    _pContext->GetDeviceContext()->ClearRenderTargetView(_pDevice->GetRenderTargetView().Get(), Colors::MidnightBlue);
    _pContext->GetDeviceContext()->ClearRenderTargetView(_pRTTRenderTargetView.Get(), Colors::Green);

    // Clear the depth buffer to 1.0 (max depth)
    _pContext->GetDeviceContext()->ClearDepthStencilView(_pDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void Home::UpdateConstantBuffer()
{

    // get the game object world transform
    XMMATRIX mGO = XMLoadFloat4x4(&_pObjectCube->World);

    // store this and the view / projection in a constant buffer for the vertex shader to use
    ConstantBuffer cb1;
    cb1.mWorld = XMMatrixTranspose(mGO);
    cb1.mView = XMMatrixTranspose(_pCamera->CalculateViewMatrix());
    cb1.mProjection = XMMatrixTranspose(_pCamera->CalculateProjectionMatrix());
    cb1.EyePosW = XMFLOAT3(_pCamera->GetUp().x , _pCamera->GetUp().y , _pCamera->GetUp().z);

    _pContext->GetDeviceContext()->UpdateSubresource(_pDevice->GetConstantBuffer().Get(), 0, nullptr, &cb1, 0, 0);
   
    //I Store Lighting values in constant buffer function since we pass the light property values to the constant buffer, we can change this eventual;ly

    _Lighting.Enabled = static_cast<int>(true);
    _Lighting.LightType = SpotLight;
    _Lighting.Color = XMFLOAT4(Colors::White);
    _Lighting.SpotAngle = XMConvertToRadians(1.0f);
    _Lighting.ConstantAttenuation = 1.0f;
    _Lighting.LinearAttenuation = 1;
    _Lighting.QuadraticAttenuation = 0.5;
    


    XMFLOAT4 LightPosition = XMFLOAT4(_pCamera->GetPosition().x, _pCamera->GetPosition().y, _pCamera->GetPosition().z, 0.0f);
    XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&_Lighting.Direction, LightDirection);


    _Lighting_Properties.EyePosition = LightPosition;

    _Lighting_Properties.Lights[0] = _Lighting;


    _pContext->GetDeviceContext()->UpdateSubresource(_pDevice->GetLightConstantBuffer().Get(), 0, nullptr, &_Lighting_Properties, 0, 0);



}
void Home::Draw()
{

    _pObjectCube->Draw( _pDevice, _pContext->GetDeviceContext().Get());
}





