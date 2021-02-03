#include "MeshObject.h"

MeshObject::MeshObject(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
	transform = ZSTransform();
}
