#pragma once
#include "Object.h"
class ObjectCube : public Object
{
public:
	ObjectCube()
	{
		_ObjectProperties = new ObjectProperties();
	}
	HRESULT InitMesh(ID3D11Device* device, ID3D11DeviceContext* context) override;
	 void InitialiseShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path) override;

	 void Draw(Device* device, ID3D11DeviceContext* device_context) override ;

	

	 void Update(ID3D11Device* device, ID3D11DeviceContext* device_context) override;
};

