#include "Application.h"




Application::Application()
{
	
	_pVertexBufferCube = nullptr;
	_pIndexBufferCube = nullptr;
	_pVertexBufferPyramid = nullptr;
	_pIndexBufferPyramid = nullptr;
	


	cubeView = true;
	pyramidView = false;


}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	appGFX = new ApplicationGraphics();

	cube = new CubeObject();
	cube->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	cube->GenerateTexture(L"Textures/Crate_COLOR.dds");


	appGFX->Initialize(hInstance, nCmdShow);

	

	return S_OK;
}

//ID3D11Device* Application::GetDevice()
//{
//	return _pd3dDevice;
//}


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

	hrCube = appGFX->GetDevice()->CreateBuffer(&bdCube, &InitDataC, &_pVertexBufferCube);

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

	hrPyramid = appGFX->GetDevice()->CreateBuffer(&bdPyramid, &InitDataP, &_pVertexBufferPyramid);

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

	appGFX->GetDevice()->CreateBuffer(&bdGridV, &InitDataGridV, &_pVertexBufferGrid);

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
	appGFX->GetDevice()->CreateBuffer(&bdGridI, &InitDataGridI, &_pIndexBufferGrid);

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
	hrCube = appGFX->GetDevice()->CreateBuffer(&bdCube, &InitData, &_pIndexBufferCube);

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
	hrPyramid = appGFX->GetDevice()->CreateBuffer(&bdPyramid, &InitDataP, &_pIndexBufferPyramid);

	if (FAILED(hrPyramid))
		return hrPyramid;

	return S_OK;
}




void Application::Cleanup()
{
	
	if (_pVertexBufferCube) _pVertexBufferCube->Release();
	if (_pIndexBufferCube) _pIndexBufferCube->Release();
	if (_pVertexBufferCube) _pVertexBufferPyramid->Release();
	if (_pIndexBufferCube) _pIndexBufferPyramid->Release();
	


}

void Application::Update()
{
	// Update our time
	static float t = appGFX->TimerUpdate(t);

	




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

	appGFX->UpdateWireFrame();

	//Switch between pyramids and cubes
	if (GetAsyncKeyState(VK_F3) && cubeView == false)
	{
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		appGFX->SetIndexBuffer(_pIndexBufferCube);
		appGFX->SetVertexBuffer(_pVertexBufferCube);
		cubeView = true;
		pyramidView = false;
	}
	if (GetAsyncKeyState(VK_F4) && pyramidView == false)
	{
		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;

		appGFX->SetIndexBuffer(_pIndexBufferPyramid);
		appGFX->SetVertexBuffer(_pIndexBufferPyramid);

		pyramidView = true;
		cubeView = false;
	}

}

void Application::Draw()
{

	cube->Draw();

	//RE IMPLEMENT HARD CODED SHAPES ///////////////////////////////////////////////////////////////////////
	/*XMFLOAT4X4 world = appGFX->GetWorld();
	Render all planets
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

			appGFX->GetWorld()  = XMLoadFloat4x4(&sceneObjects[i]);
			appGFX->GetCB().mWorld = XMMatrixTranspose(world);
			_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

			_pImmediateContext->DrawIndexed(18, 0, 0);
		}
	}*/

	//////////////////////////////////////////////////////////////////////////////////

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

	
}