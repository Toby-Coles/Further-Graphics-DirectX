#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include <windowsx.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();



	void SetLens(float fovY, float aspect, float zNear, float zFar);
	void UpdateViewMatrix();

	float GetFovX() const;
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	//Set and return methods
	void SetCameraPosition(XMFLOAT3 position);
	void Pitch(float angle);
	void RotateY(float angle);

	void SetCameraLookAtPoint(XMFLOAT3 lookAt);
	void SetCameraUp(XMFLOAT3 up);

	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	XMFLOAT3 GetCameraPosition();
	XMVECTOR GetCameraVecPosition();

	XMFLOAT3 GetCameraLookAtPoint();
	XMFLOAT3 GetCameraUp();

	void MoveFowardBack(float d);
	void Strafe(float d);

	//Get Matrices
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	//void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	float momentum;
	void InitializeCamera();

private:

	//Camera position and view volume

	XMFLOAT3 _position; //Position
	XMFLOAT3 _right; // x axis view space
	XMFLOAT3 _up; // y axis view space
	
	XMFLOAT3 _look; //z axis view space


	// Cache frustum properties.
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;


	//View and projection matrices
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;



};



