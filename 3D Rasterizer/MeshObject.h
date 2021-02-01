#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include "Shaders.h"
#include <string>

using namespace std;

class MeshObject : public SceneObject
{

public:

	shared_ptr<Mesh> mesh = nullptr;

	MeshObject(string inputfile);

	VertexShader vert;
	FragmentShader frag;

	inline void SetVertexShader(VertexShader shader) {
		vert = shader;
	}

	inline void SetFragmentShader(FragmentShader shader) {
		frag = shader;
	}

};