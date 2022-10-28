#include "ObjectQuad.h"

HRESULT ObjectQuad::InitMesh(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Create vertex buffer - Create Simple Face That Will be mapped over viewport
	std::vector<SimpleVertex> vertices =
	{

		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f,  0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f,  0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },


	};

	std::vector<WORD> indices =
	{
		0,1,2,
		0,2,3,
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = &vertices[0];
	HRESULT hr = device->CreateBuffer(&bd, &InitData, _ObjectProperties->_pVertexBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, _ObjectProperties->_pVertexBuffer.GetAddressOf(), &stride, &offset);


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &indices[0];
	hr = device->CreateBuffer(&bd, &InitData, _ObjectProperties->_pIndexBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// Set index buffer
	context->IASetIndexBuffer(_ObjectProperties->_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&sampDesc, _ObjectProperties->_pSamplerState.GetAddressOf());

	_ObjectProperties->_Material.Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_ObjectProperties->_Material.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	_ObjectProperties->_Material.Material.SpecularPower = 32.0f;
	_ObjectProperties->_Material.Material.UseTexture = true;

	// Create the material constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = device->CreateBuffer(&bd, nullptr, _ObjectProperties->_pMaterialConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		return hr;
	}


	return hr;
}

void ObjectQuad::InitialiseShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path)
{
	if (vertex_shader_path != nullptr && pixel_shader_path != nullptr)
	{

		_ObjectProperties->_pShader = std::make_unique<Shader>(device, device_context, vertex_shader_path, pixel_shader_path);

	}
}

void ObjectQuad::Draw(Device* device, ID3D11DeviceContext* device_context)
{
	device_context->VSSetShader(_ObjectProperties->_pShader.get()->GetVertexShader().Get(), nullptr, 0);
	device_context->VSSetConstantBuffers(0, 1, device->GetConstantBuffer().GetAddressOf());


	device_context->PSSetShader(_ObjectProperties->_pShader->GetPixelShader().Get(), nullptr, 0);
	device_context->PSSetConstantBuffers(2, 1, device->GetLightConstantBuffer().GetAddressOf());

	ID3D11Buffer* materialCB = _ObjectProperties->_pMaterialConstantBuffer.Get();
	device_context->PSSetConstantBuffers(1, 1, &materialCB);

	device_context->PSSetShaderResources(0, 1, _ObjectProperties->_pTextureResourceView.GetAddressOf());
	device_context->PSSetShaderResources(1, 1, _ObjectProperties->_pNormalResourceView.GetAddressOf());
	device_context->PSSetShaderResources(2, 1, _ObjectProperties->_pParallaxResourceView.GetAddressOf());
	device_context->PSSetSamplers(0, 1, _ObjectProperties->_pSamplerState.GetAddressOf());

	device_context->DrawIndexed(6, 0, 0);
}

void ObjectQuad::Update(ID3D11Device* device, ID3D11DeviceContext* device_context)
{


	XMMATRIX mTranslate = XMMatrixTranslation(_ObjectProperties->_Transformation.GetTranslate().x, _ObjectProperties->_Transformation.GetTranslate().y, _ObjectProperties->_Transformation.GetTranslate().z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(&World, world);


	device_context->UpdateSubresource(_ObjectProperties->_pMaterialConstantBuffer.Get(), 0, nullptr, &_ObjectProperties->_Material, 0, 0);
}