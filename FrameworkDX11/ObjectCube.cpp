#include "ObjectCube.h"

#define NUM_VERTICES 36
void CalculateTangentBinormalLH(SimpleVertex v0, SimpleVertex v1, SimpleVertex v2, XMFLOAT3& normal, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	XMFLOAT3 edge1(v1.Pos.x - v0.Pos.x, v1.Pos.y - v0.Pos.y, v1.Pos.z - v0.Pos.z);
	XMFLOAT3 edge2(v2.Pos.x - v0.Pos.x, v2.Pos.y - v0.Pos.y, v2.Pos.z - v0.Pos.z);

	XMFLOAT2 deltaUV1(v1.TexCoord.x - v0.TexCoord.x, v1.TexCoord.y - v0.TexCoord.y);
	XMFLOAT2 deltaUV2(v2.TexCoord.x - v0.TexCoord.x, v2.TexCoord.y - v0.TexCoord.y);

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	XMVECTOR tn = XMLoadFloat3(&tangent);
	tn = XMVector3Normalize(tn);
	XMStoreFloat3(&tangent, tn);

	binormal.x = f * (deltaUV1.x * edge2.x - deltaUV2.x * edge1.x);
	binormal.y = f * (deltaUV1.x * edge2.y - deltaUV2.x * edge1.y);
	binormal.z = f * (deltaUV1.x * edge2.z - deltaUV2.x * edge1.z);

	tn = XMLoadFloat3(&binormal);
	tn = XMVector3Normalize(tn);
	XMStoreFloat3(&binormal, tn);


	XMVECTOR vv0 = XMLoadFloat3(&v0.Pos);
	XMVECTOR vv1 = XMLoadFloat3(&v1.Pos);
	XMVECTOR vv2 = XMLoadFloat3(&v2.Pos);

	XMVECTOR e0 = vv1 - vv0;
	XMVECTOR e1 = vv2 - vv0;

	XMVECTOR e01cross = XMVector3Cross(e0, e1);
	e01cross = XMVector3Normalize(e01cross);
	XMFLOAT3 normalOut;
	XMStoreFloat3(&normalOut, e01cross);
	normal = normalOut;
}


// IMPORTANT NOTE!!
// NOTE!! - this assumes each face is using its own vertices (no shared vertices)
// so the index file would look like 0,1,2,3,4 and so on
// it won't work with shared vertices as the tangent / binormal for a vertex is related to a specific face
// REFERENCE this has largely been modified from "Mathematics for 3D Game Programmming and Computer Graphics" by Eric Lengyel
void CalculateModelVectors(std::vector<SimpleVertex>& vertices, std::vector<WORD>& indices)
{
	int faceCount, i, index;



	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (int i = 0; i < 12; i++)
	{

		XMFLOAT3 tangent, binormal, normal;

		index = i * 3;
		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormalLH(vertices[indices[index + 0]], vertices[indices[index + 1]], vertices[indices[index + 2]], normal, tangent, binormal);

		// Store the normal, tangent, and binormal for this face back in the model structure.

		vertices[indices[index + 0]].Tangent = tangent;
		vertices[indices[index + 1]].Tangent = tangent;
		vertices[indices[index + 2]].Tangent = tangent;

		vertices[indices[index + 0]].BiTangent = binormal;
		vertices[indices[index + 1]].BiTangent = binormal;
		vertices[indices[index + 2]].BiTangent = binormal;

	}

}

HRESULT ObjectCube::InitMesh(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Create vertex buffer
	std::vector<SimpleVertex> vertices =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) , XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	std::vector<WORD> indices =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};
	CalculateModelVectors(vertices, indices);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
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
	bd.ByteWidth = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
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

	//// load and setup textures
	hr = CreateDDSTextureFromFile(device, L"Resources\\metal_grate_rusty_diff_4k.dds", nullptr, _ObjectProperties->_pTextureResourceView.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// load and setup textures
	hr = CreateDDSTextureFromFile(device, L"Resources\\metal_grate_rusty_nor_dx_4k.dds", nullptr, _ObjectProperties->_pNormalResourceView.GetAddressOf());
	if (FAILED(hr))
		return hr;
	// load and setup textures
	hr = CreateDDSTextureFromFile(device, L"Resources\\metal_grate_rusty_disp_4k.dds", nullptr, _ObjectProperties->_pParallaxResourceView.GetAddressOf());
	if (FAILED(hr))
		return hr;
		// load and setup textures
	//hr = CreateDDSTextureFromFile(device, L"Resources\\brickcolor.dds", nullptr, _ObjectProperties->_pTextureResourceView.GetAddressOf());
	//if (FAILED(hr))
	//	return hr;

	//// load and setup textures
	//hr = CreateDDSTextureFromFile(device, L"Resources\\bricknormals.dds", nullptr, _ObjectProperties->_pNormalResourceView.GetAddressOf());
	//if (FAILED(hr))
	//	return hr;
	//// load and setup textures
	//hr = CreateDDSTextureFromFile(device, L"Resources\\brickdisplacement.dds", nullptr, _ObjectProperties->_pParallaxResourceView.GetAddressOf());
	//if (FAILED(hr))
	//	return hr;
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

void ObjectCube::InitialiseShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path)
{
	if (vertex_shader_path != nullptr && pixel_shader_path != nullptr)
	{

		_ObjectProperties->_pShader = std::make_unique<Shader>(device, device_context, vertex_shader_path, pixel_shader_path);

	}
}

void ObjectCube::Draw(Device* device, ID3D11DeviceContext* device_context)
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

	device_context->DrawIndexed(NUM_VERTICES, 0, 0);
}

void ObjectCube::Update(ID3D11Device* device, ID3D11DeviceContext* device_context)
{


	XMMATRIX mTranslate = XMMatrixTranslation(_ObjectProperties->_Transformation.GetTranslate().x, _ObjectProperties->_Transformation.GetTranslate().y, _ObjectProperties->_Transformation.GetTranslate().z);
	XMMATRIX world = mTranslate;
	XMStoreFloat4x4(&World, world);


	device_context->UpdateSubresource(_ObjectProperties->_pMaterialConstantBuffer.Get(), 0, nullptr, &_ObjectProperties->_Material, 0, 0);
}
