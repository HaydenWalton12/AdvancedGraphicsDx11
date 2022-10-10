#include "Objects.h"


Objects::~Objects()
{
}

void Objects::InitMesh()
{
}

void Objects::InitShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path)
{
}




void Objects::Draw()
{


	_pContext->PSSetShaderResources(0, 1, _pTextureResourceView->GetAddressOf());
	_pContext->PSSetShaderResources(1, 1, _pNormalResourceView->GetAddressOf());

	_pContext->PSSetSamplers(0, 1, _pSamplerLinear->GetAddressOf());

	_pContext->DrawIndexed(36, 0, 0);
}

void Objects::Update(float t)
{
	static float cummulativeTime = 0;
	cummulativeTime += t;

	// Cube:  Rotate around origin
	XMMATRIX scale = XMMatrixScaling(ObjectScale.x, ObjectScale.y, ObjectScale.z);
	XMMATRIX translation = XMMatrixTranslation(ObjectTranslation.x, ObjectTranslation.y, ObjectTranslation.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(ObjectRotation.x, ObjectRotation.y, ObjectRotation.z);
	XMMATRIX world = scale * rotation * translation;
	XMStoreFloat4x4(&m_World, world);

	_pContext->UpdateSubresource(_pMaterialConstantBuffer->Get(), 0, nullptr, &_material, 0, 0);
}
