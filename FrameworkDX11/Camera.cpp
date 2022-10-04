#include "Camera.h"

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::CalculateViewMatrix()
{

	DirectX::XMVECTOR eye = DirectX::XMLoadFloat4(&_Position);
	DirectX::XMVECTOR at = DirectX::XMLoadFloat4(&_At);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4(&_Up);


	return	DirectX::XMMatrixLookAtLH(eye, at, up);
 
}

DirectX::XMMATRIX Camera::CalculateProjectionMatrix()
{
	return DirectX::XMMatrixPerspectiveFovLH(_FOV, _WindowWidth / _WindowHeight, _NearZ, _FarZ);

}
