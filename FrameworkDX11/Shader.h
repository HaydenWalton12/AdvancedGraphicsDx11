#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>

class Shader
{
public:

	//Constructor Overloading To Either Create Vertex/Pixel Shader
	Shader();
	//Create Vertex Shader
	Shader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* vertex_shader_path);
	//Create Pixel Shader
	Shader(ID3D11Device* device, const wchar_t* pixel_shader_path);
	Shader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* vertex_shader_path , const wchar_t* pixel_shader_path);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() { return _VertexShader; };
	Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() { return _PixelShader; };
	//ID3D11InputLayout - Describes an element expressed for the input assembler stage (this stage determines what data will be entered for later processing within rendering/dx11 pipeline).
	Microsoft::WRL::ComPtr<ID3D11InputLayout> GetVertexLayout() { return _VertexLayout; };

	void SetVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader)
	{
		_VertexShader = vertex_shader;
	}
	void SetPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader)
	{
		_PixelShader = pixel_shader;
	}
private:
	//ComPtr - Creates a smart pointer that creates an interface to the type given , gives further functionality of raw pointer managament via this function
	//Smart Pointer - Essentially wraps around a raw/primitive pointer , manages the lifetime of this pointer automatically.
	//E.G Raw Pointers are stupid, you ave to manually destroy the object when no longer used, can become tedious in code.

	//Smart Pointers Contain two pointers, the raw pointer to manage the object referenced in memory , and the "smart pointer" which manages this pointer itself.

	

	Microsoft::WRL::ComPtr<ID3D11VertexShader> _VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _VertexLayout;
	WCHAR* _File;

	HRESULT CompileShaderFromFile(const WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut);

	HRESULT hr;
};

