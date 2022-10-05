#include "Camera.h"

Camera::~Camera()
{
}

void Camera::ImGuiCameraSettings()
{
	if (ImGui::CollapsingHeader("Active Camera Controls"))
	{

		ImGui::DragFloat3("Eye", &_Position.x, 0.015f);
		ImGui::DragFloat3("LookAt", &_At.x, 0.015f);
		ImGui::DragFloat3("Up", &_Up.x, 0.005f);
		ImGui::SliderAngle("FOV", &_FOV, 5.0f, 160.0f);
		ImGui::DragFloat("Near Plane", &_NearZ, 0.01f, 0.1f, 100.0f);
		ImGui::DragFloat("Far Plane", &_FarZ, 0.1f, 0.2f, 5000.0f);

	}
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
