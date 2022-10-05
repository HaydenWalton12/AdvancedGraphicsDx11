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
#include "structures.h"

#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


#include "InitDirectX11.h"



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


	typedef vector<DrawableGameObject*> vecDrawables;
	Camera* _pCamera;
	InitDirectX11* _pInitDx11;
	void InitialiseApplication(HWND hwnd, int width, int height);
	HRESULT InitScene(int width, int height);
	void Render();
	void SetUpLightForRender();
	float CalculateDeltaTime();
};

