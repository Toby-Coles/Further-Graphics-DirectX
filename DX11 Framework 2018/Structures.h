#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>



struct SimpleVertex
{

	XMFLOAT3 Pos;
	//XMFLOAT4 Color;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

};