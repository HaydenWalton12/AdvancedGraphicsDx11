#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/*
	Cameras are used to warp the perspective of scene within worldspace, with defined confines. Their are a multitude of different camera related systems and techniques to 
	enhance the perspective ,this class will be the defining system to manipulate the camera.


*/

class Camera
{
public:

	Camera(DirectX::XMFLOAT4 position, DirectX::XMFLOAT4 at, DirectX::XMFLOAT4 up, FLOAT window_height, FLOAT window_width, float fov, FLOAT near_z, float far_Z) :
		_Position(position), _At(at), _Up(up), _WindowHeight(window_height), _WindowWidth(window_width), _NearZ(near_z), _FarZ(far_Z), _FOV(fov)
	{

	}
	~Camera();

	void ImGuiCameraSettings();
	//Get/Set World Camera Positions

	DirectX::XMFLOAT4 GetPosition() { return _Position; }
	DirectX::XMFLOAT4 GetAt() { return _At; }
	DirectX::XMFLOAT4 GetUp() { return _Up; }

	//Get Frustrum Properties
	FLOAT			  GetNearZ() { return _NearZ; }
	FLOAT			  GetFov() { return _FOV; }
	FLOAT			  GetFarZ() { return _FarZ; }

	
	void SetPosition(DirectX::XMFLOAT4 position) { _Position = position; }
	void SetAt(DirectX::XMFLOAT4 at) { _At = at; }
	void SetUp(DirectX::XMFLOAT4 up) { _Up = up; }

	void SetFOV(FLOAT fov) { _FOV = fov; }
	void SetFarZ(FLOAT far_z) { _FarZ = far_z; }
	void SetNearZ(FLOAT near_z) { _NearZ = near_z; }

	DirectX::XMMATRIX CalculateViewMatrix();
	DirectX::XMMATRIX CalculateProjectionMatrix();
private:


	//We Store In XMFloat4 since when updating view matrix , we need too accompany an additional w value.
	//Eye Coordinate  - View Space Origin
	DirectX::XMFLOAT4 _Position;
	//Look At - Z Axes
	DirectX::XMFLOAT4 _At;
	//Look Up Or Down - Y Axes
	DirectX::XMFLOAT4 _Up;

	
	
	//Frustrum Properties

	FLOAT _FOV;
	//Used To Define Camera Perspective Limits, This Will Be The Window Size
	FLOAT _WindowWidth;
	FLOAT _WindowHeight;

	//Camera Depth Values
	FLOAT _NearZ;
	FLOAT _FarZ;

	/*
		View Space is the core coordinate system attatched to the camera. The reason this is a self defined 4x4 Matrix (Theory this is defined as the view matrix, where we 
		store and manipulate the xyz values, essentially for defening perspective).

		_View Matrix =

		[ Ux , Uy , Uz , 0]
		[ Vx , Vy , Vz , 0]
		[ Wx , Wy , Wz , 0]
		[ Qx , Qy , Qz , 1]

		The bottom row is quanternions ,this is used to prevent gimble lock with the camera, since XYZ is defined in euler angles. The values have a limit in how far
		the camera can rotate, adding the quanternion prevents this issue. Euler Angles are stored as homogenous coordinates (relative to world space)

		The View Matrix is cthe byproduct of world space - > World Space -> Viewspace
		 * Reason For this is since all objects are represented in world space (local -> world)
		 * We need to convert this objects to their corresponding value in the view space , since this will be what we see via the perpsecive of the object.
		 * Essentially making the view matrix the corresponding values of the object to how they will be *VIEWED*
	*/
	DirectX::XMFLOAT4X4 _View;
	DirectX::XMFLOAT4X4 _Projection;
};

