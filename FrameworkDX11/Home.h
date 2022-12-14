#pragma once
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

#include "DrawableGameObject.h"
#include "Lighting.h"
#include "structures.h"

#include <vector>
#include "Shader.h"
#include "Camera.h"


#include "Objects.h"
#include "ObjectCube.h"

#include "Device.h"
#include "Context.h"

#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


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

	typedef vector<DrawableGameObject*> vecDrawables;
	Camera* _pCamera;
	HINSTANCE _Instance;

	Objects* _Objects;
	ObjectCube* _Cube;

	Device* _pDevice;
	Context* _pContext;
	Lighting* _Lighting;
	Light _Light;
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
};

