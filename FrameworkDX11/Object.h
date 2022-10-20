#pragma once
#include "ObjectTransformation.h"
#include "Shader.h"
#include <wrl.h>
#include <memory.h>
#include "structures.h"

#include "DDSTextureLoader.h"
#include <vector>
#include "Device.h"

#include "DeviceResources.h"

#include <iostream>

//Contains All Relevant Values Objects Need For Functionality
struct ObjectProperties
{
	//Future Reference , If I Obj Load Models
	char* _Name;

	Transformation _Transformation;
	MaterialPropertiesConstantBuffer	_Material;

	Microsoft::WRL::ComPtr <ID3D11Buffer> _pVertexBuffer;
	Microsoft::WRL::ComPtr <ID3D11Buffer> _pIndexBuffer;
	Microsoft::WRL::ComPtr <ID3D11Buffer> _pMaterialConstantBuffer;

	//Textures For Shader Calculations
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _pTextureResourceView;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _pNormalResourceView;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> _pParallaxResourceView;

	Microsoft::WRL::ComPtr <ID3D11SamplerState> _pSamplerState;
	
	std::unique_ptr<Shader> _pShader;

	void SetTransformationMatrix(XMFLOAT3 translation, XMFLOAT3 scale, XMFLOAT3 rotation)
	{
		_Transformation = Transformation(translation, scale, rotation);
	}


	//Set Transformation Values Upon XMFLOAT3
	void SetTranslation(XMFLOAT3 translation)
	{
		_Transformation.SetTranslation(translation);
	}
	void SetScale(XMFLOAT3 scale)
	{
		_Transformation.SetScale(scale);
	}
	void SetRotation(XMFLOAT3 rotation)
	{
		_Transformation.SetRotation(rotation);
	}


	
};

class Object
{
public:

	virtual HRESULT InitMesh(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual void InitialiseShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path);
	
	virtual void Draw(DeviceResources* device, ID3D11DeviceContext* device_context);
	virtual void Update(ID3D11Device* device, ID3D11DeviceContext* device_context);
	
	ObjectProperties* _ObjectProperties;




};

