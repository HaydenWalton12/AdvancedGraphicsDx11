#include "Home.h"




void Home::InitialiseApplication(HWND hwnd , HINSTANCE instance, int width , int height)
{

    _Instance = instance;

    device = new DeviceResources();
    _pContext = new Context(hwnd, width, height);

    device->CreateDevice();
    device->CreateResources();

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

    return S_OK;
    

}

//Executes Game Loop
void Home::Tick()
{
    //Handles game-world state midification, and input
    Update();
    //Renders a single frame per-tick of scene
    Render();
}

//Draws The Scene - Renders a single frame per-tick of scene
void Home::Render()
{
    float t = CalculateDeltaTime(); // capped at 60 fps
    //Dont Try and render anything before first update
    if (t == 0.0f)
    {
        return;
    }

    //Clears and sets new view for back buffer 
    Clear();

    //Render Code Placed Here ,ALL And anything regarding proeprties of rendering the scene
    Draw();
    PresentIMGui();

    Present();

}

//Updates Resources & World/ Objects
void Home::Update()
{
    //Detect Input ,Can Determine World/Resource Values That Determine Rendered Scene
    Input(_Instance);

    _pObjectCube->Update(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get());

    //Update World Resources
    UpdateConstantBuffer();
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
//Clear Function Clears RenderTarget , Default BackGround, Then Sets Default Render Target
void Home::Clear()
{
    // Clear the back buffer
    _pContext->GetDeviceContext()->ClearRenderTargetView(_pDevice->GetRenderTargetView().Get(), Colors::MidnightBlue);
    // Clear the depth view 
    _pContext->GetDeviceContext()->ClearDepthStencilView(_pDevice->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    _pContext->GetDeviceContext()->OMGetRenderTargets(1, _pDevice->GetRenderTargetView().GetAddressOf(), _pDevice->GetDepthStencilView().GetAddressOf());
}

//Present Method , presents swapchain for switching back/front buffers, critical to rendering
void Home::Present()
{

    // Present our back buffer to our front buffer , Utility Of Adding A 1
    _pContext->GetSwapChain()->Present(1, 0);
}

//Presents ImGUi , With all Given functionality to rendering
void Home::PresentIMGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Engine Simulations");
    _pCamera->ImGuiCameraSettings();
    _pObjectCube->_ObjectProperties->_pShader.get()->ImGuiShaderSettings(_pDevice->GetDevice().Get(), _pContext->GetDeviceContext().Get());
    if (ImGui::CollapsingHeader("Active Lighting Controls"))
    {

        ImGui::DragFloat("X", &_Lighting.Position.x, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Y", &_Lighting.Position.y, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Z", &_Lighting.Position.z, 0.1f, -20.0f, 20.0f);



    }

    if (ImGui::CollapsingHeader("Active Cube Controls"))
    {

        ImGui::DragFloat("X", &_pObjectCube->_ObjectProperties->_Transformation.Translation.x, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Y", &_pObjectCube->_ObjectProperties->_Transformation.Translation.y, 0.1f, -20.0f, 20.0f);
        ImGui::DragFloat("Z", &_pObjectCube->_ObjectProperties->_Transformation.Translation.z, 0.1f, -20.0f, 20.0f);


    }
    ImGui::End();
    //Render IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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
    cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
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





    XMVECTOR LightDirection = XMVectorSet(-_Lighting.Position.x, -_Lighting.Position.y, -_Lighting.Position.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&_Lighting.Direction, LightDirection);




    _Lighting_Properties.Lights[0] = _Lighting;
    _pContext->GetDeviceContext()->UpdateSubresource(_pDevice->GetLightConstantBuffer().Get(), 0, nullptr, &_Lighting_Properties, 0, 0);



}
void Home::Draw()
{

    _pObjectCube->Draw( _pDevice, _pContext->GetDeviceContext().Get());
}





