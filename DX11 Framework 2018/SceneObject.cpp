#include "SceneObject.h"

SceneObject::SceneObject(ID3D11Device* device, char* filepath, XMFLOAT3 initialPosition, XMFLOAT3 initialRotation, wchar_t* texturePath) {

	this->_pd3dDevice = device;
	mMeshData = OBJLoader::Load(filepath, this->_pd3dDevice, false);

	Initialise();
	SetPosition(initialPosition);
	SetRotation(initialRotation);
	GenerateTexture(texturePath);

	
	
	
}

//second constructor for if a model does not need to be loaded 
SceneObject::SceneObject(XMFLOAT3 initialPosition, XMFLOAT3 initialRotation) {

	Initialise();
	SetPosition(initialPosition);
	SetRotation(initialRotation);

}

SceneObject::~SceneObject()
{
}

void SceneObject::Initialise()
{
	//Set initial Values
	XMStoreFloat4x4(&mTransform, XMMatrixIdentity());
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


}

void SceneObject::GenerateTexture(wchar_t* texturePath)
{
	ID3D11ShaderResourceView* texture;
	LoadTexture(texturePath, &texture);
	mTextures.push_back(texture);
}

HRESULT SceneObject::LoadTexture(wchar_t* path, ID3D11ShaderResourceView** texture)
{
	return CreateDDSTextureFromFile(this->_pd3dDevice, path, nullptr, texture); 

}

void SceneObject:: SetPosition(XMFLOAT3 position) {
	mPosition = position;

}

void SceneObject::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}

void SceneObject::SetScale(XMFLOAT3 scale)
{
	mScale = scale;
}

void SceneObject::SetScale(float x, float y, float z)
{
	mScale.x = x;
	mScale.y = y;
	mScale.z = z;

}

void SceneObject::SetTransform(XMFLOAT4X4 transform)
{
	mTransform = transform;
}

void SceneObject::SetTransform(XMMATRIX transform)
{
	XMStoreFloat4x4(&mTransform, transform);
}



XMFLOAT4X4 SceneObject::GetTransform()
{
	return XMFLOAT4X4();
}

XMFLOAT3 SceneObject::GetPosition()
{
	return XMFLOAT3();
}

XMFLOAT3 SceneObject::GetRotation()
{
	return XMFLOAT3();
}

XMFLOAT3 SceneObject::GetScale()
{
	return XMFLOAT3();
}

void SceneObject::Draw()
{

}

void SceneObject::Update()
{
	UpdateTransforms();
}

//Apply and update object transforms
XMMATRIX SceneObject::UpdateTransforms() {

	XMMATRIX transform = XMLoadFloat4x4(&mTransform);
	XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	XMMATRIX position = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z);
	XMMATRIX finalTransform = XMMatrixMultiply(scale, position) * rotation;

	XMStoreFloat4x4(&mTransform, finalTransform);
	return finalTransform;

}



