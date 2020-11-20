#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include <windowsx.h>
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

class ApplicationGraphics
{
public:
	ApplicationGraphics();
	~ApplicationGraphics();

	//Initialization
	HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitShadersAndInputLayout();

	//Buffers
	void SetVertexBuffer(ID3D11Buffer* buffer);
	void SetIndexBuffer(ID3D11Buffer* buffer);
	void UpdateConstantBufferVariables();
	void ClearBackBuffer();

	//Drawing
	void Draw(unsigned int indexCount);
	void Present();

	//Wireframe
	void UpdateWireFrame();
	
	float TimerUpdate(float t);


	//Shaders
	void SetShaders(ID3D11VertexShader* vs, ID3D11PixelShader* ps);

	//Getters
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	
	ConstantBuffer cb;
	ConstantBuffer GetCB();
	XMFLOAT4X4 GetWorld();

private:
	void Cleanup();

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);


	//Initialisation
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();


	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*			 _pd3dDevice;
	ID3D11DeviceContext*	 _pImmediateContext;
	IDXGISwapChain*			 _pSwapChain;
	ID3D11RenderTargetView*	_pRenderTargetView;
	ID3D11Buffer*			 _pConstantBuffer;
	XMFLOAT4X4              _world;
	XMFLOAT4X4				_world2;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	ID3D11InputLayout* _pVertexLayout;

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

	bool wireFrame;
	float keyPressTimer;
	float keyCoolDown;

	//Depth buffer variables
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	UINT _WindowHeight;
	UINT _WindowWidth;

	//Wireframe
	ID3D11RasterizerState* _wireFrame;

	//Texture Sampler
	ID3D11SamplerState* _pSamplerLinear = nullptr;



};

