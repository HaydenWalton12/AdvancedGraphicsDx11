#include "Home.h"
// ***************************************************************************************
// InitWorld - Should Be relative To Home
// ***************************************************************************************
HRESULT InitWorld(int width, int height)
{
    //Init Camera
    _pCamera = new Camera(XMFLOAT4(0.0f, 0.0f, -3.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f), g_viewHeight, g_viewWidth, XM_PIDIV2, 0.01f, 100.0f);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Render a frame 
//--------------------------------------------------------------------------------------
void Render()
{
    float t = calculateDeltaTime(); // capped at 60 fps
    if (t == 0.0f)
        return;

    // Clear the back buffer
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);

    // Clear the depth buffer to 1.0 (max depth)
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Update the cube transform, material etc. 
    g_GameObject.update(t, g_pImmediateContext);

    // get the game object world transform
    XMMATRIX mGO = XMLoadFloat4x4(g_GameObject.getTransform());

    // store this and the view / projection in a constant buffer for the vertex shader to use
    ConstantBuffer cb1;
    cb1.mWorld = XMMatrixTranspose(mGO);
    cb1.mView = XMMatrixTranspose(g_Camera->CalculateViewMatrix());
    cb1.mProjection = XMMatrixTranspose(g_Camera->CalculateProjectionMatrix());
    cb1.vOutputColor = XMFLOAT4(0, 0, 0, 0);
    g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb1, 0, 0);


    setupLightForRender();

    // Render the cube
    g_pImmediateContext->VSSetShader(g_GameObject._shader->GetVertexShader().Get(), nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

    g_pImmediateContext->PSSetShader(g_GameObject._shader->GetPixelShader().Get(), nullptr, 0);
    g_pImmediateContext->PSSetConstantBuffers(2, 1, &g_pLightConstantBuffer);
    ID3D11Buffer* materialCB = g_GameObject.getMaterialConstantBuffer();
    g_pImmediateContext->PSSetConstantBuffers(1, 1, &materialCB);
    g_GameObject.draw(g_pImmediateContext);

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
    g_pSwapChain->Present(0, 0);
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        break;
    }
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void setupLightForRender()
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
    XMFLOAT4 LightPosition(g_EyePosition);
    light.Position = LightPosition;
    XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&light.Direction, LightDirection);

    LightPropertiesConstantBuffer lightProperties;
    lightProperties.EyePosition = LightPosition;
    lightProperties.Lights[0] = light;
    g_pImmediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
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
    XMFLOAT4 LightPosition(XMFLOAT4);
    light.Position = LightPosition;
    XMVECTOR LightDirection = XMVectorSet(-LightPosition.x, -LightPosition.y, -LightPosition.z, 0.0f);
    LightDirection = XMVector3Normalize(LightDirection);
    XMStoreFloat4(&light.Direction, LightDirection);

    LightPropertiesConstantBuffer lightProperties;
    lightProperties.EyePosition = LightPosition;
    lightProperties.Lights[0] = light;
    g_pImmediateContext->UpdateSubresource(g_pLightConstantBuffer, 0, nullptr, &lightProperties, 0, 0);
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
