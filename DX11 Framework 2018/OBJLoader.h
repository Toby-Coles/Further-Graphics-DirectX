#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <fstream>		//For loading in an external file
#include <vector>		//For storing the XMFLOAT3/2 variables
#include <map>	
#include "Structures.h"//For fast searching when re-creating the index buffer

using namespace DirectX;

struct MeshData
{
	ID3D11Buffer * VertexBuffer;
	ID3D11Buffer * IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

//struct SimpleVertex
//{
//	XMFLOAT3 Pos;
//	XMFLOAT3 Normal;
//	XMFLOAT2 TexC;
//
//	bool operator<(const SimpleVertex other) const
//	{
//		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
//	};
//};

namespace OBJLoader
{
	//The only method you'll need to call
	MeshData Load(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords = true);

	//Helper methods for the above method
	//Searhes to see if a similar vertex already exists in the buffer -- if true, we re-use that index
	bool FindSimilarVertex(const SimpleVertex& vertex, std::map<SimpleVertex, unsigned short>& vertToIndexMap, unsigned short& index);

	//Re-creates a single index buffer from the 3 given in the OBJ file
	void CreateIndices(const std::vector<XMFLOAT3>& inVertices, const std::vector<XMFLOAT2>& inTexCoords, const std::vector<XMFLOAT3>& inNormals, std::vector<unsigned short>& outIndices, std::vector<XMFLOAT3>& outVertices, std::vector<XMFLOAT2>& outTexCoords, std::vector<XMFLOAT3>& outNormals);
};