#pragma once
#include "OBJLoader.h"
#include "SceneObject.h"

class SkyMap : public SceneObject
{
public:
	SkyMap();
	~SkyMap();

private:
	MeshData skyMeshData;

};

