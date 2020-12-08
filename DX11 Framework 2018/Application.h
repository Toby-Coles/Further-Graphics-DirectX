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


using namespace DirectX;

class Application
{
private:
	
	//Hard Coded shape values
	ID3D11Buffer*           _pVertexBufferCube;
	ID3D11Buffer*           _pIndexBufferCube;
	ID3D11Buffer*			_pVertexBufferPyramid;
	ID3D11Buffer*			_pIndexBufferPyramid;
	ID3D11Buffer*			_pVertexBufferGrid;
	ID3D11Buffer*			_pIndexBufferGrid;


	SimpleVertex simpleVertex;

	//Texturing
	ID3D11ShaderResourceView* p_TextureRV = nullptr;
	ID3D11ShaderResourceView* p_SpecularTexture = nullptr;

	
	bool cubeView;
	bool pyramidView;

	void GenerateGridPlain(float width, float depth, UINT m, UINT n);
	XMFLOAT3 CalculateNormals(XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3);
	void UpdateCameraControlls(float deltaTime);
	
	ApplicationGraphics* appGFX;
	SceneObject* skyMap;

	//Scene Objects
	std::vector<XMFLOAT4X4> sceneObjects;
	std::vector<XMFLOAT4X4> asteroidBelt;
	XMFLOAT4X4 gridPlane;

	SceneObject* cube;
	Camera* camera1;

private:
	

	void Cleanup();
	

	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	TimeKeep* timer;

	//Scene Objects
	std::vector<SceneObject> applicationObjects;

	float rotationSpeed;
	float rotation = 0.0f;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	
	

	void Update();
	void Draw();
};

