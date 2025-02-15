#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "SceneObject.h"
#include "ApplicationGraphics.h"
#include "Time.h"
#include "Camera.h"
#include "CubeObject.h"
#include "GroundPlane.h"
#include "SetUpGUI.h"

using namespace DirectX;

class Application
{
private:
	
	//Hard Coded shape values
	ID3D11Buffer*           _pVertexBufferCube;
	ID3D11Buffer*           _pIndexBufferCube;
	ID3D11Buffer*			_pVertexBufferPyramid;
	ID3D11Buffer*			_pIndexBufferPyramid;
	
	SimpleVertex simpleVertex;

	//Texturing
	ID3D11ShaderResourceView* p_TextureRV = nullptr;
	ID3D11ShaderResourceView* p_SpecularTexture = nullptr;

	//Update Controls
	void UpdateShipControlls(float deltaTime);
	void UpdateCameraControlls(float deltaTime);
	
	ApplicationGraphics* appGFX;
	SceneObject* skyMap;

	//Scene Objects (old, for hard coded values)
	//std::vector<XMFLOAT4X4> sceneObjects;

	XMFLOAT4X4 gridPlane;

	std::vector<SceneObject*> worldSceneObjects;

	//Scene Objects
	GroundPlane* plane;
	SceneObject* cube;
	SceneObject* earth;
	SceneObject* ship;
	SceneObject* shipPlayer;

	UserInterface ui;

	Camera* camera1;
	Camera* camera2;
	Camera* camera3;
private:
	
	bool isWireFrame = false;
	void Cleanup();
	bool showGridPlane;

	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	TimeKeep* timer;

	//Scene Objects
	std::vector<SceneObject> applicationObjects;

	float rotationSpeed;
	float earthRotationSpeed;

	float earthRotation = 0.0f;
	float rotation = 0.0f;
	XMFLOAT3 offset = XMFLOAT3(0.0f, 3.0f, 5.0f);

	float moveSpeed;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void ShowSceneUI();

	

	void Update();
	void Draw();
};

