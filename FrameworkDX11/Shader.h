#pragma once

#include <wrl.h>
#include <d3d11.h>

class Shader
{
private:
	//ComPtr - Creates a smart pointer that creates an interface to the type given , gives further functionality of raw pointer managament via this function
	//Smart Pointer - Essentially wraps around a raw/primitive pointer , manages the lifetime of this pointer automatically.
	//E.G Raw Pointers are stupid, you ave to manually destroy the object when no longer used, can become tedious in code.
	
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _VertexLayout;

	HRESULT CompileShaderFromFile(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut);
public:

	Shader(ID3D11Device* device, WCHAR* vertex_shader_path  , D3D11_INPUT_ELEMENT_DESC* vertex_layout)

	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() { return _VertexShader; };
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() { return _PixelShader; };

	//ID3D11InputLayout - Describes an element expressed for the input assembler stage (this stage determines what data will be entered for later processing within rendering/dx11 pipeline).
	Microsoft::WRL::ComPtr<ID3D11InputLayout> GetVertexLayout() { return _VertexLayout; };
};

