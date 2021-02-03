#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Shaders.h"
#include <string>

using namespace std;

class MeshObject : public SceneObject
{

public:

	shared_ptr<Mesh> mesh = nullptr;
	vector<Material> materials;

	MeshObject(shared_ptr<Mesh> mesh);

};