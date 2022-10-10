#pragma once
#include "Objects.h"
class ObjectCube : public Objects
{
public:

	void InitMesh() override;
	void InitShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path) override;

private:

};

