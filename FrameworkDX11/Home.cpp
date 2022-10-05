#include "Home.h"


DrawableGameObject		g_GameObject;


void Home::InitialiseApplication(HWND hwnd , int width , int height)
{

    _pInitDx11 = new InitDirectX11(hwnd , width , height);
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
       
        g_GameObject.initMesh(_pInitDx11->GetDevice().Get(), _pInitDx11->GetDeviceContext().Get());
        g_GameObject.initialise_shader(_pInitDx11->GetDevice().Get(), _pInitDx11->GetDeviceContext().Get(), L"shader.fx", L"shader.fx");

        return S_OK;
    

}

void Home::Render()
{
    float t = CalculateDeltaTime(); // capped at 60 fps
    if (t == 0.0f)
        return;

    // Clear the back buffer
    _pInitDx11->GetDeviceContext()->ClearRenderTargetView(_pInitDx11->GetRenderTargetView().Get(), Colors::MidnightBlue);

    // Clear the depth buffer to 1.0 (max depth)
    _pInitDx11->GetDeviceContext()->ClearDepthStencilView(_pInitDx11->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Update the cube transform, material etc. 
    g_GameObject.update(t, _pInitDx11->GetDeviceContext().Get());

    // get the game object world transform
    XMMATRIX mGO = XMLoadFloat4x4(g_GameObject.getTransform());

    // store this and the view / projection in a constant buffer for the vertex shader to use
    ConstantBuffer cb1;
    cb1.mWorld = XMMatrixTranspose(mGO);
    cb1.mView = XMMatrixTranspose(_pCamera->CalculateViewMatrix());
    cb1.mProjection = XMMatrixTranspose(_pCamera->CalculateProjectionMatrix());
    cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
    _pInitDx11->GetDeviceContext()->UpdateSubresource(_pInitDx11->GetConstantBuffer().Get(), 0, nullptr, &cb1, 0, 0);


    SetUpLightForRender();

    // Render the cube
    _pInitDx11->GetDeviceContext()->VSSetShader(g_GameObject._shader->GetVertexShader().Get(), nullptr, 0);
    _pInitDx11->GetDeviceContext()->VSSetConstantBuffers(0, 1, _pInitDx11->GetConstantBuffer().GetAddressOf());

    _pInitDx11->GetDeviceContext()->PSSetShader(g_GameObject._shader->GetPixelShader().Get(), nullptr, 0);
    _pInitDx11->GetDeviceContext()->PSSetConstantBuffers(2, 1, _pInitDx11->GetLightConstantBuffer().GetAddressOf());
    ID3D11Buffer* materialCB = g_GameObject.getMaterialConstantBuffer();
    _pInitDx11->GetDeviceContext()->PSSetConstantBuffers(1, 1, &materialCB);
    g_GameObject.draw(_pInitDx11->GetDeviceContext().Get());

    //Im GUI Draw Order , Must Be after dx11 draw order
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Physics Engine Simulations");
    //Menu Displayed After Simulation Starts
    ImGui::Text("About : This is a simple engine demonstrating the approximation of physics properties.\n\n\n\n");
    ImGui::End();

    //Render IMGUI
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present our back buffer to our front buffer
    _pInitDx11->GetSwapChain()->Present(0, 0);
}

void Home::SetUpLightForRender()
{
    Light light;
    light.Enabled = static_cast<int>(true);
    light.LightType = PointLight;
    light.Color = XMFLOAT4(Colors::White);
    light.SpotAngle = XMConvertToRadians(45.0f);
    light.ConstantAttenuation = 1.0f;
    light.LinearAttenuation = 1;
    light.QuadraticAttenuation = 1;


    // set up the light
    XMFLOAT4 LightPosition = XMFLOAT4(0.0f , 0.0f ,-5.0f ,0.0f);
    light.Position = LightPosition;
    XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&light.Direction, LightDirection);

    LightPropertiesConstantBuffer lightProperties;
    lightProperties.EyePosition = LightPosition;
    lightProperties.Lights[0] = light;
    _pInitDx11->GetDeviceContext()->UpdateSubresource(_pInitDx11->GetLightConstantBuffer().Get(), 0, nullptr, &lightProperties, 0, 0);
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
