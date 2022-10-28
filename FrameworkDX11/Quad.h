#include "Object.h"
class ObjectQuad : public Object
{
public:
	ObjectQuad()
	{
		_ObjectProperties = new ObjectProperties();
		_ObjectProperties->_Transformation = Transformation(XMFLOAT3(0.0f, -1.2f, 0.0f), XMFLOAT3(5.0f, 5.0f, 5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	}
	HRESULT InitMesh(ID3D11Device* device, ID3D11DeviceContext* context  , Context* context_class) override;
	void InitialiseShader(ID3D11Device* device, ID3D11DeviceContext* device_context, const wchar_t* pixel_shader_path, const wchar_t* vertex_shader_path) override;

	void Draw(Device* device, ID3D11DeviceContext* device_context) override;

	ID3D11Texture2D* _pRTTRrenderTargetTexture;
	// render target
	ID3D11RenderTargetView* _pRTTRenderTargetView;
	// shader resource view for the texture
	ID3D11ShaderResourceView* _pRTTShaderResourceView;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	XMFLOAT4X4 World;

	void Update(ID3D11Device* device, ID3D11DeviceContext* device_context) override;
};