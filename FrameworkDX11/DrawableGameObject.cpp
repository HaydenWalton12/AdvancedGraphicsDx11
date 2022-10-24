//#include "DrawableGameObject.h"
//#include "Device.h"
//#include <vector>
//
//using namespace std;
//using namespace DirectX;
//
//#define NUM_VERTICES 36
//
//DrawableGameObject::DrawableGameObject()
//{
//	m_pVertexBuffer = nullptr;
//	m_pIndexBuffer = nullptr;
//	m_pTextureResourceView = nullptr;
//	m_pSamplerLinear = nullptr;
//
//	// Initialize the world matrix
//	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
//}
//
//
//DrawableGameObject::~DrawableGameObject()
//{
//	cleanup();
//}
//
//void DrawableGameObject::cleanup()
//{
//	if (m_pVertexBuffer)
//		m_pVertexBuffer->Release();
//	m_pVertexBuffer = nullptr;
//
//	if (m_pIndexBuffer)
//		m_pIndexBuffer->Release();
//	m_pIndexBuffer = nullptr;
//
//	if (m_pTextureResourceView)
//		m_pTextureResourceView->Release();
//	m_pTextureResourceView = nullptr;
//
//	if (m_pSamplerLinear)
//		m_pSamplerLinear->Release();
//	m_pSamplerLinear = nullptr;
//
//	if (m_pMaterialConstantBuffer)
//		m_pMaterialConstantBuffer->Release();
//	m_pMaterialConstantBuffer = nullptr;
//}
//
//HRESULT DrawableGameObject::initMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
//{
//	// Create vertex buffer
//	std::vector<SimpleVertex> vertices =
//	{
//		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
//		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
//		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
//		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
//
//		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
//		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
//		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
//		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
//
//		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
//		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
//		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
//		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
//
//		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
//		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
//		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
//		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
//
//		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
//		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) , XMFLOAT2(1.0f, 1.0f) },
//		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
//		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
//
//		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
//		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
//		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
//		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
//	};
//
//	std::vector<WORD> indices =
//	{
//		3,1,0,
//		2,1,3,
//
//		6,4,5,
//		7,4,6,
//
//		11,9,8,
//		10,9,11,
//
//		14,12,13,
//		15,12,14,
//
//		19,17,16,
//		18,17,19,
//
//		22,20,21,
//		23,20,22
//	};
//	//CalculateModelVectors(vertices, indices);
//
//	D3D11_BUFFER_DESC bd = {};
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(SimpleVertex) * 24;
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA InitData = {};
//	InitData.pSysMem = &vertices[0];
//	HRESULT hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	// Set vertex buffer
//	UINT stride = sizeof(SimpleVertex);
//	UINT offset = 0;
//	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
//
//
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(WORD) * NUM_VERTICES;        // 36 vertices needed for 12 triangles in a triangle list
//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	InitData.pSysMem = &indices[0];
//	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	// Set index buffer
//	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
//
//	// Set primitive topology
//	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// load and setup textures
//	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\TerracottaBaseColor.dds", nullptr, &m_pTextureResourceView);
//	if (FAILED(hr))
//		return hr;
//
//	// load and setup textures
//	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Terracottanormal.dds", nullptr, &m_pNormalResourceView);
//	if (FAILED(hr))
//		return hr;
//		// load and setup textures
//	hr = CreateDDSTextureFromFile(pd3dDevice, L"Resources\\Heightmap.dds", nullptr, &m_pDissplacementResourceView);
//	if (FAILED(hr))
//		return hr;
//	D3D11_SAMPLER_DESC sampDesc;
//	ZeroMemory(&sampDesc, sizeof(sampDesc));
//	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
//	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//	sampDesc.MinLOD = 0;
//	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
//	hr = pd3dDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
//
//	m_material.Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//	m_material.Material.Specular = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
//	m_material.Material.SpecularPower = 32.0f;
//	m_material.Material.UseTexture = true;
//
//	// Create the material constant buffer
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(MaterialPropertiesConstantBuffer);
//	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	bd.CPUAccessFlags = 0;
//	hr = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pMaterialConstantBuffer);
//
//	if (FAILED(hr))
//		return hr;
//
//	return hr;
//}
//
//void DrawableGameObject::initialise_shader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path)
//{
//	
//
//	//Old Solution
//	/*if (vertex_shader_path != nullptr && pixel_shader_path != nullptr)
//	{
//		_shader = std::make_unique<Shader>(device , pixel_shader_path);
//		Microsoft::WRL::ComPtr<ID3D11PixelShader> temp = _shader.get()->GetPixelShader();
//		_shader = std::make_unique<Shader>(device, device_context, vertex_shader_path);
//		_shader.get()->SetPixelShader(temp);
//	}*/
//
//	//New Solution
//	if (vertex_shader_path != nullptr && pixel_shader_path != nullptr)
//	{
//
//		_shader = std::make_unique<Shader>(device, device_context, vertex_shader_path , pixel_shader_path);
//
//	}
//}
//
//void DrawableGameObject::setPosition(XMFLOAT3 position)
//{
//	m_position = position;
//}
//
//void DrawableGameObject::update(float t, ID3D11DeviceContext* pContext)
//{
//	static float cummulativeTime = 0;
//	cummulativeTime += t;
//
//	// Cube:  Rotate around origin
//	XMMATRIX mScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
//	XMMATRIX mTranslate = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
//	XMMATRIX world = mTranslate * mScale ;
//	XMStoreFloat4x4(&m_World, world);
//
//	pContext->UpdateSubresource(m_pMaterialConstantBuffer, 0, nullptr, &m_material, 0, 0);
//}
//
//void DrawableGameObject::draw(ID3D11DeviceContext* pContext)
//{
//	
//	pContext->PSSetShaderResources(0, 1, &m_pTextureResourceView);
//	pContext->PSSetShaderResources(1, 1, &m_pNormalResourceView);
//	pContext->PSSetShaderResources(2, 1, &m_pDissplacementResourceView);
//	pContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
//
//	pContext->DrawIndexed(NUM_VERTICES, 0, 0);
//}
