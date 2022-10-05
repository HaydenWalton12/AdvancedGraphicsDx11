#include "Home.h"


DrawableGameObject		g_GameObject;




void Home::InitialiseApplication(HWND hwnd , HINSTANCE instance, int width , int height)
{

    _Instance = instance;

    _pDevice = new Device(hwnd, width, height);
    _pContext = new Context(hwnd, width, height);
    _pDevice->CreateDevice(_pContext , _pContext->GetDeviceContext().Get() , _pContext->GetFactory1().Get());
    _pContext->SetSwapChain(_pDevice->GetDevice().Get());
    _pDevice->CreateRenderTargetView(_pContext->GetSwapChain().Get());

    _pContext->SetRenderTargetView(_pDevice->GetRenderTargetView().Get(), _pDevice->GetDepthStencilView().Get());
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
        //Initialise Camera
    int 	g_viewWidth = 720;
    int     g_viewHeight = 1280;
        _pCamera = new Camera(XMFLOAT4(0.0f, 0.0f, -3.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), g_viewWidth, g_viewHeight, XM_PIDIV2, 0.01f, 100.0f);
       
        g_GameObject.initMesh(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get());
        g_GameObject.initialise_shader(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get(), L"shader.fx", L"shader.fx");

        return S_OK;
    

}

void Home::Render()
{
    //Im GUI Draw Order , Must Be after dx11 draw order

    ClearRenderTarget();


    Input(_Instance);


    //float t = CalculateDeltaTime(); // capped at 60 fps
    //if (t == 0.0f)
    //    return;


    // Update the cube transform, material etc. 
    g_GameObject.update(0.0016, _pContext->GetDeviceContext().Get());
    UpdateConstantBuffer();
    Draw();



    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Engine Simulations");
    _pCamera->ImGuiCameraSettings();
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
    // Clear the depth buffer to 1.0 (max depth)
    _pContext->GetDeviceContext()->ClearDepthStencilView(_pDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void Home::UpdateConstantBuffer()
{

    // get the game object world transform
    XMMATRIX mGO = XMLoadFloat4x4(g_GameObject.getTransform());

    // store this and the view / projection in a constant buffer for the vertex shader to use
    ConstantBuffer cb1;
    cb1.mWorld = (mGO);
    cb1.mView = XMMatrixTranspose(_pCamera->CalculateViewMatrix());
    cb1.mProjection = XMMatrixTranspose(_pCamera->CalculateProjectionMatrix());
    cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
    _pContext->GetDeviceContext()->UpdateSubresource(_pDevice->GetConstantBuffer().Get(), 0, nullptr, &cb1, 0, 0);
   
    //I Store Lighting values in constant buffer function since we pass the light property values to the constant buffer, we can change this eventual;ly
    Light light;
    light.Enabled = static_cast<int>(true);
    light.LightType = PointLight;
    light.Color = XMFLOAT4(Colors::White);
    light.SpotAngle = XMConvertToRadians(45.0f);
    light.ConstantAttenuation = 1.0f;
    light.LinearAttenuation = 1;
    light.QuadraticAttenuation = 1;


    // set up the light
    XMFLOAT4 LightPosition = XMFLOAT4(0.0f, 0.0f, -5.0f, 0.0f);
    light.Position = LightPosition;
    XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&light.Direction, LightDirection);

    LightPropertiesConstantBuffer lightProperties;
    lightProperties.EyePosition = LightPosition;
    lightProperties.Lights[0] = light;
    _pContext->GetDeviceContext()->UpdateSubresource(_pDevice->GetLightConstantBuffer().Get(), 0, nullptr, &lightProperties, 0, 0);


}
void Home::Draw()
{
    // Render the cube
    _pContext->GetDeviceContext()->VSSetShader(g_GameObject._shader->GetVertexShader().Get(), nullptr, 0);
    _pContext->GetDeviceContext()->VSSetConstantBuffers(0, 1, _pDevice->GetConstantBuffer().GetAddressOf());

    _pContext->GetDeviceContext()->PSSetShader(g_GameObject._shader->GetPixelShader().Get(), nullptr, 0);
    _pContext->GetDeviceContext()->PSSetConstantBuffers(2, 1, _pDevice->GetLightConstantBuffer().GetAddressOf());
    ID3D11Buffer* materialCB = g_GameObject.getMaterialConstantBuffer();
    _pContext->GetDeviceContext()->PSSetConstantBuffers(1, 1, &materialCB);
    UpdateConstantBuffer();
    g_GameObject.draw(_pContext->GetDeviceContext().Get());
}





