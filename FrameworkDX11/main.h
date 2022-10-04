#pragma once
#include "Home.h"

//Centre of scene loop , window and D3D11 implementation
class Main
{
public:
	Main();
	~Main();

	Camera* _Camera;
	typedef vector<DrawableGameObject*> vecDrawables;
};
