#pragma once
#include <windows.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

class ImGuiManager
{
public:

	ImGuiManager(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
	{
		//Setup ImGui

	}

	~ImGuiManager()
	{



	}

	void Start()
	{



	}

	void End()
	{

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}
};

