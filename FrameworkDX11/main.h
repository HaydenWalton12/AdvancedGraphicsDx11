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

using namespace std;



//Centre of scene loop , window and D3D11 implementation
class Main
{
public:
	Main();
	~Main();


	typedef vector<DrawableGameObject*> vecDrawables;
};
