#include "MeshObject.h"

MeshObject::MeshObject(string inputfile)
{
	mesh = make_shared<Mesh>();
	LoadMesh(inputfile, mesh);
	transform = ZSTransform();
	vert = DEFAULT_VERTEX_SHADER;
	frag = DEFAULT_FRAGMENT_SHADER;
}
