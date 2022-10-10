#pragma once
#include "Device.h"
#include "DDSTextureLoader.h"
#include <memory.h>
#include "Shader.h"
#include <iostream>
struct Light
{
	//Creates Default light
	Light()
		: Position(0.0f, 0.0f, 0.0f, 1.0f)
		, Direction(0.0f, 0.0f, 1.0f, 0.0f)
		, Color(1.0f, 1.0f, 1.0f, 1.0f)
		, SpotAngle(1.5)
		, ConstantAttenuation(1.0f)
		, LinearAttenuation(0.0f)
		, QuadraticAttenuation(0.0f)
		, LightType(DirectionalLight)
		, Enabled(0)
	{}


	Light(XMFLOAT4 position, XMFLOAT4 direction, XMFLOAT4 colour, float spot_angle, float constant_attenuation, float linear_attenuation, float quadratic_attenuation
		, int light_type, int enabled)
		: Position(position.x, position.y, position.z, position.w)
		, Direction(direction.x, direction.y, direction.z, direction.w)
		, Color(colour.x, colour.y, colour.z, colour.w)
		, SpotAngle(spot_angle)
		, ConstantAttenuation(constant_attenuation)
		, LinearAttenuation(linear_attenuation)
		, QuadraticAttenuation(quadratic_attenuation)
		, LightType(light_type)
		, Enabled(enabled)
	{}

	DirectX::XMFLOAT4    Position;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4    Direction;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4    Color;
	//----------------------------------- (16 byte boundary)
	float       SpotAngle;
	float       ConstantAttenuation;
	float       LinearAttenuation;
	float       QuadraticAttenuation;
	//----------------------------------- (16 byte boundary)
	int         LightType;
	int         Enabled;
	// Add some padding to make this struct size a multiple of 16 bytes.
	int         Padding[2];
	//----------------------------------- (16 byte boundary)
};  // Total:                              80 bytes ( 5 * 16 )


struct LightPropertiesConstantBuffer
{
	LightPropertiesConstantBuffer()
		: EyePosition(0, 0, 0, 1)
		, GlobalAmbient(0.2f, 0.2f, 0.8f, 1.0f)
	{}

	DirectX::XMFLOAT4   EyePosition;
	//----------------------------------- (16 byte boundary)
	DirectX::XMFLOAT4   GlobalAmbient;
	//----------------------------------- (16 byte boundary)
	Light               Lights[MAX_LIGHTS]; // 80 * 8 bytes
};  // Total:                                  672 bytes (42 * 16)
class Lighting
{
public:
	std::unique_ptr<Shader> _shader;

	HRESULT InitMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);

	void setPosition(XMFLOAT3 position);

	void update(float t, ID3D11DeviceContext* pContext);

	void draw(ID3D11DeviceContext* pContext);

	void initialise_shader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path);
	
	XMFLOAT4X4							m_World;

	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11ShaderResourceView* m_pTextureResourceView;
	ID3D11ShaderResourceView* m_pNormalResourceView;
	ID3D11SamplerState* m_pSamplerLinear;
	MaterialPropertiesConstantBuffer	m_material;
	ID3D11Buffer* m_pMaterialConstantBuffer = nullptr;
	XMFLOAT3							m_position;

private:




};

