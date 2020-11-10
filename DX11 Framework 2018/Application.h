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



using namespace DirectX;

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT3 LightVecW;
	float dataBreak;

	XMFLOAT4 AmbientLight;
	XMFLOAT4 AmbientMtrl;

	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float SpecularPower;
	XMFLOAT3 EyePosW;
};




class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBufferCube;
	ID3D11Buffer*           _pIndexBufferCube;
	ID3D11Buffer*			_pVertexBufferPyramid;
	ID3D11Buffer*			_pIndexBufferPyramid;
	ID3D11Buffer*			_pVertexBufferGrid;
	ID3D11Buffer*			_pIndexBufferGrid;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world;
	XMFLOAT4X4				_world2;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;

	//Diffuse Light
	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;

	//Ambient Light
	XMFLOAT4 ambientLight;
	XMFLOAT4 ambientMateral;

	//specular Light
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 EyePosW;

	//Texturing
	ID3D11ShaderResourceView* p_TextureRV = nullptr;
	ID3D11ShaderResourceView* p_SpecularTexture = nullptr;


	//Texture Sampler
	ID3D11SamplerState* _pSamplerLinear = nullptr;


	bool cubeView;
	bool pyramidView;

	void GenerateGridPlain(float width, float depth, UINT m, UINT n);
	XMFLOAT3 CalculateNormals(XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3);
	 
	//Depth buffer variables
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;


	//Scene Objects
	std::vector<XMFLOAT4X4> sceneObjects;
	std::vector<XMFLOAT4X4> asteroidBelt;
	XMFLOAT4X4 gridPlane;


	//Wireframe
	ID3D11RasterizerState* _wireFrame;


private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

	float keyPressTimer;
	float keyCoolDown;
	bool wireFrame;


public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

