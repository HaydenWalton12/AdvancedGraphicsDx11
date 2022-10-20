#pragma once
#include "Object.h"
class ObjectQuad : public Object
{
public:
	ObjectQuad()
	{
		_ObjectProperties = new ObjectProperties();
		_ObjectProperties->_Transformation = Transformation(XMFLOAT3(0.0f, -1.2f, 0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	HRESULT InitMesh(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void InitialiseShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path) override;

	void Draw(Device* device, ID3D11DeviceContext* device_context) override;


	XMFLOAT4X4 World;

	void Update(ID3D11Device* device, ID3D11DeviceContext* device_context) override;
};