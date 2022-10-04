#pragma once

#include <windows.h>
#include <windowsx.h>

#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>

#include "DrawableGameObject.h"
#include "structures.h"

#include "InitDirectX11.h"

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


//Centre Of Entire FrameWork that will collate all elements together to create functional system
//initialises ImGui & DX11 Too
class Home
{
public:
	Home()
	{}
	~Home()
	{

	}
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pLightConstantBuffer;
	Camera* _pCamera;
	InitDirectX11* _pInitDx11;
	HRESULT InitWorld(int width, int height);
	void Render();
	void SetUpLightForRender();
	float CalculateDeltaTime();
};

