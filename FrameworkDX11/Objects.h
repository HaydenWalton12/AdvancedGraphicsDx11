#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>
#include "structures.h"
#include <memory.h>
#include "Shader.h"
#include "Device.h"
using namespace DirectX;

class Objects
{
public:
	Objects(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext) : _pDevice(pd3dDevice) , _pContext(pContext)
	{

	}
	~Objects();

	virtual void InitMesh();
	virtual void InitShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path);
	void Draw();
	void Update(float t);
	void SetScale(float x, float y, float z)
	{

		ObjectScale = XMFLOAT3(x, y, z);
	}
	void SetRotation(float x, float y, float z) 
	{
		ObjectRotation = XMFLOAT3(x, y, z);
	}
	void SetTranslation(float x, float y, float z)
	{
		ObjectTranslation = XMFLOAT3(x, y, z);

	}
	virtual Microsoft::WRL::ComPtr <ID3D11Buffer> getVertexBuffer() { return _pVertexBuffer; }
	virtual Microsoft::WRL::ComPtr <ID3D11Buffer> getIndexBuffer() { return _pIndexBuffer; }
	virtual Microsoft::WRL::ComPtr < ID3D11ShaderResourceView> getTextureResourceView() { return _pTextureResourceView; }
	virtual XMFLOAT4X4* getTransform() { return &m_World; }
	virtual Microsoft::WRL::ComPtr < ID3D11SamplerState> getTextureSamplerState() { return _pSamplerLinear; }
	virtual Microsoft::WRL::ComPtr <ID3D11Buffer> getMaterialConstantBuffer() { return _pMaterialConstantBuffer; }
private:




protected:
	ID3D11Device* _pDevice;
	ID3D11DeviceContext* _pContext;

	Microsoft::WRL::ComPtr <ID3D11Buffer>* _pVertexBuffer;
	Microsoft::WRL::ComPtr <ID3D11Buffer>* _pIndexBuffer;
	Microsoft::WRL::ComPtr <ID3D11Buffer>* _pMaterialConstantBuffer ;

	XMFLOAT3 ObjectTranslation;
	XMFLOAT3 ObjectRotation;
	XMFLOAT3 ObjectScale;

	XMFLOAT4X4							m_World;

	Microsoft::WRL::ComPtr < ID3D11ShaderResourceView>* _pTextureResourceView;
	Microsoft::WRL::ComPtr < ID3D11ShaderResourceView>* _pNormalResourceView;
	Microsoft::WRL::ComPtr < ID3D11SamplerState>* _pSamplerLinear;
	MaterialPropertiesConstantBuffer	_material;

	std::unique_ptr<Shader> _shader;
};

