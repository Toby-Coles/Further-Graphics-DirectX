#include "Application.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBufferCube = nullptr;
	_pIndexBufferCube = nullptr;
	_pVertexBufferPyramid = nullptr;
	_pIndexBufferPyramid = nullptr;
	_pConstantBuffer = nullptr;


	cubeView = true;
	pyramidView = false;

	wireFrame = false;


}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return E_FAIL;
	}

	RECT rc;
	GetClientRect(_hWnd, &rc);
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if (FAILED(InitDevice()))
	{
		Cleanup();

		return E_FAIL;
	}

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -4.5f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f));

	EyePosW = XMFLOAT3(0.0f, 0.0f, -4.5);

	//Initialise Lighting

	//Light Direction from surface
	lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);

	//Diffuse material properties (RGBA)
	diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);

	//Diffuse Light Colour (RGBA)
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//Ambient Light
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	ambientMateral = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	//Specular Light
	specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	specularMaterial = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	specularPower = 10.0f;

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}
XMFLOAT3 Application::CalculateNormals(XMFLOAT3 vertex1, XMFLOAT3 vertex2, XMFLOAT3 vertex3)
{
	XMFLOAT3 normal;

	//Compute two vectors that lie on the edges
	float uX = vertex2.x - vertex1.x;
	float uY = vertex2.y - vertex1.y;
	float uZ = vertex2.z - vertex1.z;

	float vX = vertex3.x - vertex1.x;
	float vY = vertex3.y - vertex1.y;
	float vZ = vertex3.z - vertex1.z;

	XMFLOAT3 u = XMFLOAT3(uX, uY, uZ);
	XMFLOAT3 v = XMFLOAT3(vX, vY, vZ);

	//Calculate the cross product
	float crossX = u.y * v.z - u.z * v.y;
	float crossY = u.z * v.x - u.x * v.z;
	float crossZ = u.z * v.y - u.y * v.x;


	//XMFLOAT3 cross = XMFLOAT3(crossX, crossY, crossZ);

	//Calculate magnitude
	//float mag1 = sqrt((vertex1.x * vertex1.x) + (vertex2.x * vertex2.x) + (vertex3.x + vertex3.x));
	//float mag2 = sqrt((vertex1.y * vertex1.y) + (vertex2.y * vertex2.y) + (vertex3.y + vertex3.y));
	//float mag3 = sqrt((vertex1.z * vertex1.z) + (vertex2.z * vertex2.z) + (vertex3.z + vertex3.z));

	float magnitude = sqrt(((vertex1.x * vertex1.x) + (vertex1.y * vertex1.y) + (vertex1.z * vertex1.z)) + ((vertex2.x * vertex2.x) + (vertex2.y * vertex2.y) + (vertex2.z * vertex2.z)) + ((vertex3.x + vertex3.x) + (vertex3.y + vertex3.y) + (vertex3.z + vertex3.z)));
	// float magnitude = mag1 + mag2 + mag3;

	 //Normalise result of cross prod
	float normalisedCrossX = crossX / magnitude;
	float normalisedCrossY = crossY / magnitude;
	float normalisedCrossZ = crossZ / magnitude;

	normal = XMFLOAT3(normalisedCrossX, normalisedCrossY, normalisedCrossZ);

	return normal;


}

HRESULT Application::InitVertexBuffer()
{
	//GenerateGridPlain(3.0f, 3.0f, 6, 4);

	HRESULT hrCube;
	SimpleVertex verticesCube[] = {
	
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 0.0) },	//Top Back Left
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Back Right
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Back Left
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Back Left
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Back Right
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 1.0) },  	//Bottom Back Left
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	//Top Back Right
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Back Right
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Top Back Right
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) },  	//Bottom Front Right
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Left
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Right
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Right
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 1.0) },  	//Bottom Front Left
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	//Top Front Left
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	//Top Back Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	//Bottom Front Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	//Bottom Front Left
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	//Top Back Left
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) }, 	//Bottom Back Left
		{ XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	//Top Front Left
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Top Back Left
		{ XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Top Back Left
		{ XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Top Front Right
		{ XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 1.0) },  	//Top Back Left
		{ XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 0.0) },  	//Bottom Back Left
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Bottom Back Right
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Left
		{ XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	//Bottom Front Left
		{ XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	//Bottom Back Right
		{ XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 1.0) },	//Bottom Front Right


	};

	D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));
	bdCube.Usage = D3D11_USAGE_DEFAULT;
	bdCube.ByteWidth = sizeof(verticesCube);
	bdCube.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdCube.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataC;
	ZeroMemory(&InitDataC, sizeof(InitDataC));
	InitDataC.pSysMem = verticesCube;

	hrCube = _pd3dDevice->CreateBuffer(&bdCube, &InitDataC, &_pVertexBufferCube);

	if (FAILED(hrCube))
		return hrCube;

	//Create vertex buffer for pyramid
	HRESULT hrPyramid;
	SimpleVertex verticesPyramid[]{
		//Base
		{XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, -1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f)},
		{XMFLOAT3(1.0f,-1.0f,1.0f)	, XMFLOAT3(1.0f,-1.0f,1.0f)},
		{XMFLOAT3(1.0f,-1.0f,-1.0f)	, XMFLOAT3(1.0f,-1.0f,-1.0f)},

		//Point
		{XMFLOAT3(0.0f,1.0f,0.0f) ,XMFLOAT3(0.0f,1.0f,0.0f) },

	};

	D3D11_BUFFER_DESC bdPyramid;
	ZeroMemory(&bdPyramid, sizeof(bdPyramid));
	bdPyramid.Usage = D3D11_USAGE_DEFAULT;
	bdPyramid.ByteWidth = sizeof(verticesPyramid);
	bdPyramid.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdPyramid.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataP;
	ZeroMemory(&InitDataP, sizeof(InitDataP));
	InitDataP.pSysMem = verticesPyramid;

	hrPyramid = _pd3dDevice->CreateBuffer(&bdPyramid, &InitDataP, &_pVertexBufferPyramid);

	if (FAILED(hrPyramid))
		return hrPyramid;




	return S_OK;
}

void Application::GenerateGridPlain(float width, float depth, UINT m, UINT n) {
	// std::vector<D3D11_BUFFER_DESC> meshData;

	UINT vertexCount = m * n;
	UINT faceCount = (m - 1) * (n - 1);

	//Create Vertices
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	std::vector<SimpleVertex> gridVertices;
	gridVertices.resize(vertexCount);

	for (UINT i = 0; i < m; i++)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; j++)
		{
			float x = -halfWidth + j * dx;
			gridVertices[i * n + j].Pos = XMFLOAT3(x, 0.0f, z);

		}
	}
	//Create vertex buffer for grid
	D3D11_BUFFER_DESC bdGridV;
	ZeroMemory(&bdGridV, sizeof(bdGridV));
	bdGridV.Usage = D3D11_USAGE_DEFAULT;
	bdGridV.ByteWidth = sizeof(vertexCount) * (gridVertices.size() * 2);
	bdGridV.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdGridV.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataGridV;
	ZeroMemory(&InitDataGridV, sizeof(InitDataGridV));
	InitDataGridV.pSysMem = &gridVertices[0];

	_pd3dDevice->CreateBuffer(&bdGridV, &InitDataGridV, &_pVertexBufferGrid);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//Generate Indices//
	////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<WORD> gridIndices;
	gridIndices.resize(faceCount * 3);

	//iterate over each quad and compute indices
	UINT k = 0;
	for (UINT i = 0; i < m - 1; i++)
	{
		for (UINT j = 0; j < n - 1; j++)
		{
			gridIndices[k] = i * n + j;
			gridIndices[k + 1] = i * n + j + i;
			gridIndices[k + 2] = (i + 1) * n + j;
			gridIndices[k + 3] = (i + 1) * n + j;
			gridIndices[k + 4] = i * n + j + 1;
			gridIndices[k + 5] = (i + 1) * n + j + 1;
		}
	}

	D3D11_BUFFER_DESC bdGridI;
	ZeroMemory(&bdGridI, sizeof(bdGridI));

	bdGridI.Usage = D3D11_USAGE_DEFAULT;
	bdGridI.ByteWidth = sizeof(gridIndices) * gridIndices.size();
	bdGridI.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdGridI.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataGridI;
	ZeroMemory(&InitDataGridI, sizeof(InitDataGridI));
	InitDataGridI.pSysMem = &gridIndices[0];
	_pd3dDevice->CreateBuffer(&bdGridI, &InitDataGridI, &_pIndexBufferGrid);

}



HRESULT Application::InitIndexBuffer()
{
	HRESULT hrCube;

	// Create index buffer for Cube
	WORD indicesCube[] =
	{
		0,1,2 ,3,4,5,
		6,7,8 ,9,10,11,
		12,13,14 ,15,16,17,
		18,19,20 ,21,22,23,
		24,25,26 ,27,28,29,
		30,31,32 ,33,34,35
	};

	D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));

	bdCube.Usage = D3D11_USAGE_DEFAULT;
	bdCube.ByteWidth = sizeof(indicesCube);
	bdCube.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdCube.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indicesCube;
	hrCube = _pd3dDevice->CreateBuffer(&bdCube, &InitData, &_pIndexBufferCube);

	HRESULT hrPyramid;

	//Create index buffer for Pyramid
	WORD indicesPyramid[]{
		0, 2, 1,
		1, 2, 3,
		0, 1, 4,
		1, 3, 4,
		3, 2, 4,
		2, 0, 4,
	};

	D3D11_BUFFER_DESC bdPyramid;
	ZeroMemory(&bdPyramid, sizeof(bdPyramid));

	bdPyramid.Usage = D3D11_USAGE_DEFAULT;
	bdPyramid.ByteWidth = sizeof(indicesPyramid);
	bdPyramid.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdPyramid.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataP;
	ZeroMemory(&InitDataP, sizeof(InitDataP));
	InitDataP.pSysMem = indicesPyramid;
	hrPyramid = _pd3dDevice->CreateBuffer(&bdPyramid, &InitDataP, &_pIndexBufferPyramid);

	if (FAILED(hrPyramid))
		return hrPyramid;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, nCmdShow);

	return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		if (pErrorBlob) pErrorBlob->Release();

		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT Application::InitDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _WindowWidth;
	sd.BufferDesc.Height = _WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;
	//Define the depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the depth/Stencil Buffer
	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)_WindowWidth;
	vp.Height = (FLOAT)_WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();

	// Set vertex buffer (initial)
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferCube, &stride, &offset);

	InitIndexBuffer();

	// Set index buffer (initial)
	_pImmediateContext->IASetIndexBuffer(_pIndexBufferCube, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);


	//Set up Wireframe
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	//Load texture from file
	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/Crate_COLOR.dds", nullptr, &p_TextureRV);
	_pImmediateContext->PSSetShaderResources(0, 1, &p_TextureRV);

	CreateDDSTextureFromFile(_pd3dDevice, L"Textures/Crate_SPECULAR.dds", nullptr, &p_SpecularTexture);
	_pImmediateContext->PSSetShaderResources(1, 1, &p_SpecularTexture);

	//Create Sampler
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&samplerDesc, &_pSamplerLinear);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);




	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Application::Cleanup()
{
	if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_pConstantBuffer) _pConstantBuffer->Release();
	if (_pVertexBufferCube) _pVertexBufferCube->Release();
	if (_pIndexBufferCube) _pIndexBufferCube->Release();
	if (_pVertexBufferCube) _pVertexBufferPyramid->Release();
	if (_pIndexBufferCube) _pIndexBufferPyramid->Release();
	if (_pVertexLayout) _pVertexLayout->Release();
	if (_pVertexShader) _pVertexShader->Release();
	if (_pPixelShader) _pPixelShader->Release();
	if (_pRenderTargetView) _pRenderTargetView->Release();
	if (_pSwapChain) _pSwapChain->Release();
	if (_pImmediateContext) _pImmediateContext->Release();
	if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();


}

void Application::Update()
{
	// Update our time
	static float t = 0.0f;

	if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();

		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;

		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}




	//Scene object creation
	sceneObjects.resize(5);

	XMMATRIX sol = XMMatrixIdentity();
	sol = XMMatrixMultiply(sol, XMMatrixScaling(0.7, 0.7, 0.7) * XMMatrixTranslation(0, 0, 0) * XMMatrixRotationRollPitchYaw(t * 1, t * 0.5, 0));
	XMStoreFloat4x4(&sceneObjects[0], sol);

	XMMATRIX planet1 = XMMatrixIdentity();
	planet1 = XMMatrixMultiply(planet1, XMMatrixScaling(0.25, 0.25, 0.25) * XMMatrixTranslation(1.5, 0, 0) * XMMatrixRotationRollPitchYaw(t * 0.8, t * 1, 0));
	XMStoreFloat4x4(&sceneObjects[1], planet1);

	XMMATRIX planet2 = XMMatrixIdentity();
	planet2 = XMMatrixMultiply(planet2, XMMatrixScaling(0.3, 0.3, 0.3) * XMMatrixTranslation(3, 0, 0) * XMMatrixRotationRollPitchYaw(t * 0.5, t * 1.4, 0));
	XMStoreFloat4x4(&sceneObjects[3], planet2);

	XMMATRIX planet1_moon = XMMatrixIdentity();
	planet1_moon = XMMatrixMultiply(planet1_moon, XMMatrixTranslation(5.5, 0, 0) * XMMatrixScaling(0.1, 0.1, 0.1) * XMMatrixRotationRollPitchYaw(0, t * 2, 0) * XMMatrixRotationRollPitchYaw(0, t * 1, 0) * XMMatrixTranslation(1.5, 0, 0) * XMMatrixRotationRollPitchYaw(0, t * 1, 0));
	XMStoreFloat4x4(&sceneObjects[2], planet1_moon);

	XMMATRIX planet2_moon = XMMatrixIdentity();
	planet2_moon = XMMatrixMultiply(planet2_moon, XMMatrixTranslation(6.5, 0, 0) * XMMatrixScaling(0.1, 0.1, 0.1) * XMMatrixRotationRollPitchYaw(0, t * 2, 0) * XMMatrixRotationRollPitchYaw(0, t * 1, 0) * XMMatrixTranslation(3, 0, 0) * XMMatrixRotationRollPitchYaw(0, t * 1.4, 0));
	XMStoreFloat4x4(&sceneObjects[4], planet2_moon);

	////Plane object creation
	//XMMATRIX plane = XMMatrixIdentity();
	//plane = XMMatrixMultiply(plane, XMMatrixScaling(0.5, 0.5, 0.5) * XMMatrixTranslation(0, 0, 3));
	//XMStoreFloat4x4(&gridPlane, plane);


	//Enable/disable wireframe

	if (GetAsyncKeyState(VK_F1))
	{

		if (wireFrame == false)
		{
			_pImmediateContext->RSSetState(_wireFrame);
			wireFrame = true;
		}
	}

	if (GetAsyncKeyState(VK_F2))
	{
		if (wireFrame == true)
		{
			_pImmediateContext->RSSetState(nullptr);
			wireFrame = false;
		}
	}

	//Switch between pyramids and cubes
	if (GetAsyncKeyState(VK_F3) && cubeView == false)
	{
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		_pImmediateContext->IASetIndexBuffer(_pIndexBufferCube, DXGI_FORMAT_R16_UINT, 0);
		_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferCube, &stride, &offset);
		cubeView = true;
		pyramidView = false;
	}
	if (GetAsyncKeyState(VK_F4) && pyramidView == false)
	{
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		_pImmediateContext->IASetIndexBuffer(_pIndexBufferPyramid, DXGI_FORMAT_R16_UINT, 0);
		_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferCube, &stride, &offset);

		pyramidView = true;
		cubeView = false;
	}

}

void Application::Draw()
{
	// Clear the back buffer
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	// Update variables

	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	cb.DiffuseMtrl = diffuseMaterial;
	cb.DiffuseLight = diffuseLight;
	cb.LightVecW = lightDirection;
	cb.AmbientLight = ambientLight;
	cb.AmbientMtrl = ambientMateral;
	cb.SpecularMtrl = specularMaterial;
	cb.SpecularLight = specularLight;
	cb.SpecularPower = specularPower;
	cb.EyePosW = EyePosW; 

	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	//Set Shaders
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	//Render all planets
	if (cubeView == true)
	{
		for (int i = 0; i < 5; i++)
		{

			world = XMLoadFloat4x4(&sceneObjects[i]);
			cb.mWorld = XMMatrixTranspose(world);
			_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

			_pImmediateContext->DrawIndexed(36, 0, 0);
		}
	}
	else if (pyramidView == true)
	{
		for (int i = 0; i < 5; i++)
		{

			world = XMLoadFloat4x4(&sceneObjects[i]);
			cb.mWorld = XMMatrixTranspose(world);
			_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

			_pImmediateContext->DrawIndexed(18, 0, 0);
		}
	}

	////Draw Grid plane
	//UINT stride = sizeof(SimpleVertex);
	//UINT offset = 0;
	//_pImmediateContext->IASetIndexBuffer(_pIndexBufferGrid, DXGI_FORMAT_R16_UINT, 0);
	//_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBufferGrid, &stride, &offset);

	//Draw the plane
 /*   world = XMLoadFloat4x4(&gridPlane);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(96, 0, 0);*/

	/*for (int i = 0; i < 100; i++)
	{
		world = XMLoadFloat4x4(&asteroidBelt[i]);
		cb.mWorld = XMMatrixTranspose(world);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		_pImmediateContext->DrawIndexed(36, 0, 0);
	}*/
	// Present our back buffer to our front buffer

	_pSwapChain->Present(0, 0);
}