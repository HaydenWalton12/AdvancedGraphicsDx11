#pragma once

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>


#include <windows.h>
#include <windowsx.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>


#include "Object.h"
#include "ObjectCube.h"
#include "structures.h"

#include <vector>
#include "Shader.h"
#include "Camera.h"


#include "Device.h"
#include "Context.h"


//Centre Of Entire FrameWork that will collate all elements together to create functional system
//Collates Elements To Create Executable Runtime
class Home
{
public:
	Home()
	{}
	~Home()
	{

	}

	IDirectInputDevice8* DIKeyBoard;
	IDirectInputDevice8* DIMouse;

	DIMOUSESTATE MouseLastState;
	LPDIRECTINPUT8 DirectInput;

	Camera* _pCamera;
	HINSTANCE _Instance;

	Device* _pDevice;
	Context* _pContext;
	Light _Lighting;
	LightPropertiesConstantBuffer _Lighting_Properties;
	void InitialiseApplication(HWND hwnd, HINSTANCE instance, int width, int height);
	HRESULT InitScene(int width, int height);
	void Render();

	float CalculateDeltaTime();
	void Input(HINSTANCE instance);
	void InitDirectInput(HINSTANCE instance);
	void ClearRenderTarget();
	void UpdateConstantBuffer();
	void Draw();

	std::unique_ptr<Shader> _QuadShader;


	Microsoft::WRL::ComPtr<ID3D11InputLayout> _QuadVertexLayout;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRTTRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pRTTShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pRTTexture;
	Microsoft::WRL::ComPtr <ID3D11Buffer> _QuadVB;
	Microsoft::WRL::ComPtr <ID3D11Buffer> _QuadIB;


	void DetectInput()
	{


	}
	ObjectCube* _pObjectCube;
};

