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

using namespace DirectX;



class DrawableGameObject
{
public:
	DrawableGameObject();
	~DrawableGameObject();

	void cleanup();

	HRESULT								initMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	void								initialise_shader(ID3D11Device* device, ID3D11DeviceContext* device_context , const wchar_t* pixel_shader_path , const wchar_t* vertex_shader_path);


	void								update(float t, ID3D11DeviceContext* pContext);
	void								draw(ID3D11DeviceContext* pContext);
	ID3D11Buffer*						getVertexBuffer() { return m_pVertexBuffer; }
	ID3D11Buffer*						getIndexBuffer() { return m_pIndexBuffer; }
	ID3D11ShaderResourceView**			getTextureResourceView() { return &m_pTextureResourceView; 	}
	XMFLOAT4X4*							getTransform() { return &m_World; }
	ID3D11SamplerState**				getTextureSamplerState() { return &m_pSamplerLinear; }
	ID3D11Buffer*						getMaterialConstantBuffer() { return m_pMaterialConstantBuffer;}
	void								setPosition(XMFLOAT3 position);
	//Unique Pointer
	//Smart Pointer derivative , manages annother object through a pointer , when the object is out of scope , pointer is automatically disposed
	std::unique_ptr<Shader> _shader;
private:
	
	XMFLOAT4X4							m_World;

	ID3D11Buffer*						m_pVertexBuffer;
	ID3D11Buffer*						m_pIndexBuffer;
	ID3D11ShaderResourceView*			m_pTextureResourceView;
	ID3D11ShaderResourceView*			m_pNormalResourceView;
	ID3D11SamplerState *				m_pSamplerLinear;
	MaterialPropertiesConstantBuffer	m_material;
	ID3D11Buffer*						m_pMaterialConstantBuffer = nullptr;
	XMFLOAT3							m_position;


protected:

	//Shared Pointer - 
	//Smart pointer derivative that retains an "shared" ownership of an object via a pointer, may own the same object.
	//This allows us to share the management of the pointer during its lifetime, contains all functionality of a smart pointer too.

	//In OOP sense , we can share the management of the pointer to child classes that have access to this
	//std::shared_ptr<Shader> _shader;

};

