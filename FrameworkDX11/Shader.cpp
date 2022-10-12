#include "Shader.h"


Shader::Shader()
{
}

Shader::Shader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* vertex_shader_path)
{
	ID3D11InputLayout* vertex_layout;


	vertex_shader_path;
	ID3DBlob* pVSBlob = nullptr;

	CompileShaderFromFile(vertex_shader_path, "VS", "vs_4_0", &pVSBlob);

	// Create the pixel shader
	hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_VertexShader);

	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The FX File Vertex Shader Cannot be Compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);

	}


	// Currently Defined Vertex Shader Layout In accordance to our writeable "shader.fx" code in hlsl , allows us to correctly calultate the per vertex calculations
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT num_elements = ARRAYSIZE(layout);

	// Create the input layout - Describes layout of input buffer data within input assembler stage.
	device->CreateInputLayout(layout, num_elements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &vertex_layout);
	pVSBlob->Release();


	//Needs Rendering Command System
	device_context->IASetInputLayout(vertex_layout);

}

Shader::Shader(ID3D11Device* device, const wchar_t* pixel_shader_path)
{
	ID3DBlob* pPSBlob = nullptr;
	
	CompileShaderFromFile(pixel_shader_path, "PS", "ps_4_0", &pPSBlob);
	

	// Create the pixel shader
	hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_PixelShader);

	//Check Error Method
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX File Pixel Shader Cannot be Compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}

}

Shader::Shader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* vertex_shader_path, const wchar_t* pixel_shader_path)
{
	_Pixel_File = pixel_shader_path;
	_Vertex_File = vertex_shader_path;

	CreateVertexShader(device , device_context);
	CreatePixelShader(device, device_context);

}

void Shader::CreateVertexShader(ID3D11Device* device, ID3D11DeviceContext* device_context)
{
	ID3D11InputLayout* vertex_layout;

	ID3DBlob* pVSBlob = nullptr;
	CompileShaderFromFile(_Vertex_File, "VS", "vs_4_0", &pVSBlob);


	// Create the pixel shader
	hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_VertexShader);

	if (FAILED(hr))

	{
		MessageBox(nullptr, L"The FX File Vertex Shader Cannot be Compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);

	}


	// Currently Defined Vertex Shader Layout In accordance to our writeable "shader.fx" code in hlsl , allows us to correctly calultate the per vertex calculations
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT num_elements = ARRAYSIZE(layout);

	// Create the input layout - Describes layout of input buffer data within input assembler stage.
	device->CreateInputLayout(layout, num_elements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &vertex_layout);
	pVSBlob->Release();


	//Needs Rendering Command System
	device_context->IASetInputLayout(vertex_layout);
}
void Shader::CreatePixelShader(ID3D11Device* device, ID3D11DeviceContext* device_context)
{
	ID3DBlob* pPSBlob = nullptr;

	CompileShaderFromFile(_Pixel_File, "PS", "ps_4_0", &pPSBlob);

	// Create the pixel shader
	hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_PixelShader);

	//Check Error Method
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The FX File Pixel Shader Cannot be Compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
	}
}

void Shader::ImGuiShaderSettings(ID3D11Device* device, ID3D11DeviceContext* device_context)
{

	
	if (ImGui::CollapsingHeader("Shader Controls"))
	{
		//This wouldnt work since setting limit to sizeof vertex/pixel input will automatically be zero since they are currently empty
		//ImGui::Text("Vertex Shader File");
		//ImGui::InputText("##Vertex Shader Input", &vertex_input , sizeof(vertex_input));
		//ImGui::Text("Pixel Shader File");
		//ImGui::InputText("##Pixel Shader Input", &pixel_input, sizeof(pixel_input));

		 static char vertex_path[64] = "";
		 static char pixel_path[64] = "";
		ImGui::Text("Vertex Shader File");
		ImGui::InputText("Vertex Shader Input", vertex_path, IM_ARRAYSIZE(vertex_path));
		ImGui::Text("Pixel Shader File");
		ImGui::InputText("Pixel Shader Input", pixel_path, IM_ARRAYSIZE(pixel_path));
		if (ImGui::Button("Set New Shaders"))
		{
			wchar_t* a = new wchar_t;

			mbstowcs(a, vertex_path , 64);
			_Vertex_File = a;
			_Vertex_File = a;
			CreateVertexShader(device, device_context);
			CreatePixelShader(device, device_context);
			
		}
			


	}
}


HRESULT Shader::CompileShaderFromFile(const WCHAR* file_path, LPCSTR entry_point, LPCSTR shader_model, ID3DBlob** blob_out)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	//Used to return arbitrary-length data - In this instance it returns error messages
	ID3DBlob* pErrorBlob;

	//Compiles Shader From FIle
	hr = D3DCompileFromFile(file_path, nullptr, nullptr, entry_point, shader_model, dwShaderFlags, 0, blob_out, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		}
		if (pErrorBlob)
		{
			pErrorBlob->Release();
		}
		return hr;
	}

	return S_OK;

}
