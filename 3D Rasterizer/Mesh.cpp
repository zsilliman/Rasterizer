#define TINYOBJLOADER_IMPLEMENTATION
#include "Mesh.h"
#include "tinyobjloader.h"
#include <iostream>
#include <stdlib.h>

void LoadMesh(string inputfile, shared_ptr<Mesh> mesh)
{
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = "./Assets/Testing/"; // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(inputfile, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	//Load vertex attributes
	for (int v = 0; v < attrib.vertices.size(); v += 3) {
		Vector4d vertex;
		vertex << attrib.vertices[v + 0],
			attrib.vertices[v + 1],
			attrib.vertices[v + 2],
			1;
		mesh->Vertices.push_back(vertex);
	}
	//Load normal attributes
	for (int n = 0; n < attrib.normals.size(); n += 3) {
		Vector4d normal;
		normal << attrib.normals[n + 0],
			attrib.normals[n + 1],
			attrib.normals[n + 2],
			0;
		mesh->Normals.push_back(normal);
	}
	//Load UV attributes
	for (int v = 0; v < attrib.texcoords.size(); v += 2) {
		Vector2d uv;
		uv << attrib.texcoords[v + 0],
			attrib.texcoords[v + 1];
		mesh->UVs.push_back(uv);
	}

	// Loop over shapes
	for (int s = 0; s < shapes.size(); s++) {
		Shape shape;
		// Loop over faces(polygon)
		int index_offset = 0;
		for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			if (fv != 3) {
				std::cerr << "TinyObjReader: Face is not a triangle.";
				exit(1);
			}

			Triangle t;
			int z = f + index_offset + attrib.vertices.size();
			t.Color.r = rand() % 256;
			t.Color.g = rand() % 256;
			t.Color.b = rand() % 256;
			t.Color.a = 1;

			// Loop over vertices in the face.
			for (int v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				t.indices[v].VertexIndex = idx.vertex_index;
				t.indices[v].NormalIndex = idx.normal_index;
				t.indices[v].UVIndex = idx.texcoord_index;
			}
			index_offset += fv;
			shape.Triangles.push_back(t);
		}
		mesh->Shapes.push_back(shape);
	}

	std::cout << "\nMESH VERTICES: " << mesh->Vertices.size() << "\n";
	std::cout << "MESH NORMALS: " << mesh->Normals.size() << "\n";
	std::cout << "MESH UVs: " << mesh->UVs.size() << "\n";
}
