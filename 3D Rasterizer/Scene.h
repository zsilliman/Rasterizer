#pragma once
#include <vector>
#include "PerspectiveCamera.h"
#include "MeshObject.h"
#include "SceneTexture.h"

class Scene
{

public:

	std::vector<MeshObject> meshes;
	//std::vector<Lights> lighting;
	shared_ptr<PerspectiveCamera> camera;

};
