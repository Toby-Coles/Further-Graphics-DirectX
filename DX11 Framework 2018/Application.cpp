#include "Application.h"




Application::Application()
{
	//initialise variables 
	_pVertexBufferCube = nullptr;
	_pIndexBufferCube = nullptr;
	_pVertexBufferPyramid = nullptr;
	_pIndexBufferPyramid = nullptr;
	cubeView = true;
	pyramidView = false;
	appGFX = nullptr;
	skyMap = nullptr;
	cube = nullptr;
	earth = nullptr;
	camera1 = nullptr;

}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	//Initialise graphics object, used for referencing its functionability to be used in the application
	appGFX = new ApplicationGraphics();
	appGFX->Initialize(hInstance, nCmdShow);

	ui = UserInterface(appGFX);
	
	//Create and set variables for the camera object and set the current active camera in the program
	camera1 = new Camera();
	   

	camera1->SetCameraPosition(XMFLOAT3(0.0f, 0.0f, 15.5f));
	camera1->LookAt(camera1->GetCameraPosition(), XMFLOAT3(0.0f, 0.0f, 1.0f), camera1->GetCameraUp());
	camera1->SetLens(90.0f, 1920 /1080, 0.01f, 1000.0f);

	

	//Create the object for the crate cube in the scene (ATTEMPT REPLACE WITH LOAD IN FILE)
	cube = new SceneObject(appGFX);
	cube->LoadModelMesh("Models/cube.obj", appGFX->GetDevice());
	cube->SetPosition(XMFLOAT3(15.3f, 0.2f, 0.1f));
	cube->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	cube->SetRotation(XMFLOAT3(0.1f, 0.1f, 0.1f));
	cube->GenerateTexture(L"Textures/Crate_COLOR.dds", appGFX->GetDevice());
	cube->GenerateTexture(L"Textures/Crate_SPEC.dds", appGFX->GetDevice());

	//Create the earth object in the scene (ATTEMPT REPLACE WITH LOAD IN FILE)
	earth = new SceneObject(appGFX);
	earth->LoadModelMesh("Models/sphere2.obj", appGFX->GetDevice());
	earth->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	earth->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));
	earth->GenerateTexture(L"Textures/earth_color.dds", appGFX->GetDevice());
	earth->GenerateTexture(L"Textures/earth_spec.dds", appGFX->GetDevice());
	earth->GenerateTexture(L"Textures/earth_night.dds", appGFX->GetDevice());

	ship = new SceneObject(appGFX);
	ship->LoadModelMesh("Models/userModel.obj", appGFX->GetDevice());
	ship->SetPosition(XMFLOAT3(0.0f, 0.0f, - 25.0f));
	ship->SetScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
	ship->GenerateTexture(L"Textures/shipTex.dss", appGFX->GetDevice());

	shipPlayer = new SceneObject(appGFX);
	shipPlayer->LoadModelMesh("Models/userModelRotated.obj", appGFX->GetDevice());
	shipPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 40.0f));
	shipPlayer->SetScale(XMFLOAT3(0.3f, 0.3f, 0.3f));
	shipPlayer->GenerateTexture(L"Textures/shipTex.dss", appGFX->GetDevice());
	

	camera2 = new Camera();
	camera2->SetCameraPosition(XMFLOAT3(0.0f, 0.0f, 15.0f));
	camera2->LookAt(camera2->GetCameraPosition(), XMFLOAT3(0.0f, 0.0f, 1.0f), camera2->GetCameraUp());
	camera2->SetLens(90.0f, 1920 / 1080, 0.01f, 1000.0f);
	camera2->UpdateViewMatrix();

	//Initialise the view matrix for the camera
	camera1->UpdateViewMatrix();
	appGFX->SetEyePosW(camera1->GetCameraPosition());

	
	//Create the object and initialise the variables for the skybox(skysphere)
	skyMap = new SceneObject(appGFX);
	skyMap->LoadModelMesh("Models/sphere.obj", appGFX->GetDevice());
	skyMap->SetPosition(XMFLOAT3(0.0f, 0.0f, 5.5f));
	skyMap->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	skyMap->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	skyMap->GenerateTexture(L"Textures/stars_map.dds", appGFX->GetDevice());

	plane = new GroundPlane(appGFX);
	plane->GeneratePlane(30.0f, 30.0f, 8, 8);
	plane->SetPosition (XMFLOAT3(0.0f, -10.0f, 0.0f));
	plane->GenerateTexture(L"Textures/planeSurface.dds", appGFX->GetDevice());

	
	//Initialise the timer in the program
	timer = new TimeKeep();
	timer->Reset();
	timer->Start();

	moveSpeed = 3.0f;
	

	//Set rotation values
	rotation = 0.0f;
	rotationSpeed = 0.5f;
	earthRotationSpeed = 0.1f;
	appGFX->SetCamera(camera1);
	return S_OK;
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

	//Hard coded vertex buffer for a cube - NOT IMPLEMENTED INTO CODE HERE TO DEMONSTRATE UNDERSTANDING AND ABILITY
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

	//Create the buffer for the cube
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


HRESULT Application::InitIndexBuffer()
{
	//Hard coded index buffers - NOT IMPLEMENTED INTO SCENE HERE TO PROVIDE UNDERSTANDING OF CONCEPT
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
	//Cleanup memory
	if (_pVertexBufferCube) _pVertexBufferCube->Release();
	if (_pIndexBufferCube) _pIndexBufferCube->Release();
	if (_pVertexBufferCube) _pVertexBufferPyramid->Release();
	if (_pIndexBufferCube) _pIndexBufferPyramid->Release();
	if (p_TextureRV) p_TextureRV->Release();
	if (p_SpecularTexture) p_SpecularTexture->Release();
	
	
	
}

void Application::Update()
{
	appGFX->UpdateLighting();
	// Update our time
	timer->Tick();
	float deltaTime = timer->DeltaTime();
	rotation += (rotationSpeed * deltaTime);
	earthRotation += (earthRotationSpeed * deltaTime);

	// HARD CODED SCENE OBJECT IMPLEMENTATION
	////Scene object creation
	//sceneObjects.resize(5);
	//XMMATRIX sol = XMMatrixIdentity();
	//sol = XMMatrixMultiply(sol, XMMatrixScaling(0.7, 0.7, 0.7) * XMMatrixTranslation(0, 0, 0) * XMMatrixRotationRollPitchYaw(t * 1, t * 0.5, 0));
	//XMStoreFloat4x4(&sceneObjects[0], sol);
	//XMMATRIX planet1 = XMMatrixIdentity();
	//planet1 = XMMatrixMultiply(planet1, XMMatrixScaling(0.25, 0.25, 0.25) * XMMatrixTranslation(1.5, 0, 0) * XMMatrixRotationRollPitchYaw(t * 0.8, t * 1, 0));
	//XMStoreFloat4x4(&sceneObjects[1], planet1);
	//XMMATRIX planet2 = XMMatrixIdentity();
	//planet2 = XMMatrixMultiply(planet2, XMMatrixScaling(0.3, 0.3, 0.3) * XMMatrixTranslation(3, 0, 0) * XMMatrixRotationRollPitchYaw(t * 0.5, t * 1.4, 0));
	//XMStoreFloat4x4(&sceneObjects[3], planet2);
	//XMMATRIX planet1_moon = XMMatrixIdentity();
	//planet1_moon = XMMatrixMultiply(planet1_moon, XMMatrixTranslation(5.5, 0, 0) * XMMatrixScaling(0.1, 0.1, 0.1) * XMMatrixRotationRollPitchYaw(0, t * 2, 0) * XMMatrixRotationRollPitchYaw(0, t * 1, 0) * XMMatrixTranslation(1.5, 0, 0) * XMMatrixRotationRollPitchYaw(0, t * 1, 0));
	//XMStoreFloat4x4(&sceneObjects[2], planet1_moon);
	//XMMATRIX planet2_moon = XMMatrixIdentity();
	//planet2_moon = XMMatrixMultiply(planet2_moon, XMMatrixTranslation(6.5, 0, 0) * XMMatrixScaling(0.1, 0.1, 0.1) * XMMatrixRotationRollPitchYaw(0, t * 2, 0) * XMMatrixRotationRollPitchYaw(0, t * 1, 0) * XMMatrixTranslation(3, 0, 0) * XMMatrixRotationRollPitchYaw(0, t * 1.4, 0));
	//XMStoreFloat4x4(&sceneObjects[4], planet2_moon);
	////Plane object creation
	//XMMATRIX plane = XMMatrixIdentity();
	//plane = XMMatrixMultiply(plane, XMMatrixScaling(0.5, 0.5, 0.5) * XMMatrixTranslation(0, 0, 3));
	//XMStoreFloat4x4(&gridPlane, plane);

	
	appGFX->SetEyePosW(appGFX->GetCurrentCamera()->GetCameraPosition());

	//Update scene objects
	cube->Update();
	earth->Update();
	earth->SetRotation (XMFLOAT3(0.0f, earthRotation, 0.0f ));
	shipPlayer->Update();

	camera2->SetPosition(
		shipPlayer->GetPosition().x + offset.x,
		shipPlayer->GetPosition().y + offset.y,
		shipPlayer->GetPosition().z + offset.z );
	


	
	ship->SetRotation(XMFLOAT3(0.0f, rotation, 0.0f));
	
	ship->Update();
	
	
	skyMap->SetPosition(appGFX->GetCurrentCamera()->GetCameraPosition()); 
	
	skyMap->Update();
	plane->Update();

	cube->SetRotation(XMFLOAT3(rotation, 0.3f, 0.3f));
	isWireFrame = appGFX->UpdateWireFrame();

	//Switch between pyramids and cubes
	/*if (GetAsyncKeyState(VK_F3) && cubeView == false)
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
	}*/
	


	camera2->UpdateViewMatrix();
	//Update camera controll input polling
	
	UpdateCameraControlls(deltaTime);

	UpdateShipControlls(deltaTime);
	
}

void Application::UpdateShipControlls(float deltaTime) {
	XMFLOAT3 shipPosition = shipPlayer->GetPosition();
	XMFLOAT3 shipRotation = shipPlayer->GetRotation();

	if (GetAsyncKeyState('I'))
	{
		shipPosition.z += -moveSpeed * deltaTime;
	}
	else if (GetAsyncKeyState('K'))
	{
		shipPosition.z += moveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('J'))
	{
		shipPosition.x += moveSpeed * deltaTime;
	}
	else if (GetAsyncKeyState('L'))
	{
		shipPosition.x += -moveSpeed * deltaTime;
	}
	if (GetAsyncKeyState('O'))
	{
		shipRotation.y += moveSpeed * deltaTime;
	}

	//Position update
	if (shipPosition.x != shipPlayer->GetPosition().x
		|| shipPosition.z != shipPlayer->GetPosition().z) 
		shipPlayer->SetPosition(shipPosition);

	//Rotation Update
	if (shipRotation.x != shipPlayer->GetRotation().x
		|| shipRotation.y != shipPlayer->GetRotation().y ||
		shipRotation.z != shipPlayer->GetRotation().z)
		shipPlayer->SetRotation(shipRotation);

}
void Application::UpdateCameraControlls(float deltaTime)
{
	//Camera controlls for W, A, S and D

	//W - S
	if (GetAsyncKeyState('W')) camera1->MoveFowardBack(10.0f * deltaTime);
	else if (GetAsyncKeyState('S')) camera1->MoveFowardBack(-10.0f * deltaTime);
	
	//A - D
	if (GetAsyncKeyState('A')) camera1->Strafe(-10.0f * deltaTime);
	else if (GetAsyncKeyState('D')) camera1->Strafe(10.0f * deltaTime);
	
	//Q-E
	if (GetAsyncKeyState('Q')) camera1->RotateY(-5.0f * deltaTime);
	else if (GetAsyncKeyState('E')) camera1->RotateY(5.0f * deltaTime);
	
	//R-F
	if (GetAsyncKeyState('R')) camera1->Pitch(-5.0f * deltaTime);
	else if (GetAsyncKeyState('F')) camera1->Pitch(5.0f * deltaTime);

	// ================= Camera Selection ================= //

	if (GetAsyncKeyState('1')) appGFX->SetCamera(camera1);
	appGFX->SetEyePosW(camera1->GetCameraPosition());

	if (GetAsyncKeyState('2')) appGFX->SetCamera(camera2);
	appGFX->SetEyePosW(camera2->GetCameraPosition());

	// ================= Mouse Movement ================= //
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (isFirstMouseMove)
	{
		isFirstMouseMove = false;
		m_old_mouse_position = CursorPoint(current_mouse_pos.x, current_mouse_pos.y);
	}

	if (current_mouse_pos.x!=m_old_mouse_position.m_x || current_mouse_pos.y != m_old_mouse_position.m_y  )
	{
		//MOUSE MOVEMENT OCCURRED
		OnMouseMove(CursorPoint(current_mouse_pos.x = m_old_mouse_position.m_x, current_mouse_pos.y - m_old_mouse_position.m_y));
	}
	m_old_mouse_position = CursorPoint(current_mouse_pos.x, current_mouse_pos.y);



	//if (GetAsyncKeyState(VK_UP) && camera1->momentum > 10)
	//{
	//	camera1->momentum += 0.5f * deltaTime;
	//	camera1->MoveFowardBack(camera1->momentum);
	//}
	//else if (camera1->momentum <= 0)
	//{
	//	camera1->momentum -= 0.5f * deltaTime;
	//}

	camera1->UpdateViewMatrix();
}

void Application::OnMouseMove(const CursorPoint& delta_mouse_pos)
{

}

void Application::ShowSceneUI()
{
	XMFLOAT3 earthScale = XMFLOAT3(earth->GetScale());
	XMFLOAT3 earthPosition = XMFLOAT3(earth->GetPosition());
	XMFLOAT3 shipOrbiterScale = XMFLOAT3(ship->GetScale());
	XMFLOAT3 shipPosition = XMFLOAT3(ship->GetPosition());

	ImGui::Begin("Scene Object Control Panel");
	ImGui::Text("Earth");
	ImGui::SliderFloat("Earth Scale X", &earthScale.x, 0.0f, 50.0f);
	ImGui::SliderFloat("Earth Scale Y", &earthScale.y, 0.0f, 50.0f);
	ImGui::SliderFloat("Earth Scale Z", &earthScale.z, 0.0f, 50.0f);
	ImGui::SliderFloat("Earth Rotation", &earthRotationSpeed, 0.0f, 2.0f);
	ImGui::SliderFloat("Position X", &earthPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("Position Y", &earthPosition.y, -50.0f, 50.0f);
	ImGui::SliderFloat("Position Z", &earthPosition.z, -50.0f, 50.0f);
	ImGui::Text("SpaceShip(Orbiter)");
	ImGui::SliderFloat("Ship Scale X", &shipOrbiterScale.x, 0.0f, 50.0f);
	ImGui::SliderFloat("Ship Scale Y", &shipOrbiterScale.y, 0.0f, 50.0f);
	ImGui::SliderFloat("Ship Scale Z", &shipOrbiterScale.z, 0.0f, 50.0f);
	ImGui::SliderFloat("Ship Rotation", &rotationSpeed, 0.0f, 2.0f);
	ImGui::SliderFloat("Ship Position X", &shipPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("Ship Position Y", &shipPosition.y, -50.0f, 50.0f);
	ImGui::SliderFloat("ship Position Z", &shipPosition.z, -50.0f, 50.0f);

	ImGui::End();
	earth->SetScale(earthScale);
	earth->SetPosition(earthPosition);
	ship->SetScale(shipOrbiterScale);
	ship->SetPosition(shipPosition);
}

void Application::Draw()
{
	//Change the rasterizer state to render textures on the outside of the objects
	ui.FrameBegin();
	
	appGFX->SetSkyboxRasterizerState(false);
	
	ShowSceneUI();
	appGFX->ClearBackBuffer();
	//Draw objects
	appGFX->SetPixelShader(appGFX->GetScenePixelShader());

	appGFX->SetWireFrame(isWireFrame);

	cube->Draw();
	earth->Draw();
	ship->Draw();
	shipPlayer->Draw();
	plane->Draw(plane->GetPlaneVb(), plane->GetPlaneIb(), plane->GetPlaneIndexCount());
	appGFX->RunLightingControls();

	//Set the rasterizer state to draw the textures on the inside of the model for the skybox

	appGFX->SetSkyboxRasterizerState(true);
	
	

	appGFX->SetPixelShader(appGFX->GetSkyboxPixelShader());

	skyMap->Draw();

	//skyMap->Draw();
	ui.FrameEnd();

	appGFX->Present();

	//HARD CODED RENDERING ///////////////////////////////////////////////////////////////////////
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