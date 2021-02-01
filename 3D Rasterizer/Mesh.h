#pragma once
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <SDL.h>
#include <memory>

using namespace Eigen;
using namespace std;

struct MeshIndex {
	int ShapeIndex;
	int VertexIndex;
	int NormalIndex;
	int UVIndex;
};

struct Triangle
{
	MeshIndex indices[3];
	SDL_Color Color;
};

struct Shape
{
	vector<Triangle> Triangles;
};

struct Mesh
{
	vector<Shape> Shapes;
	vector<Vector4d, Eigen::aligned_allocator<Vector4d>> Vertices;
	vector<Vector4d, Eigen::aligned_allocator<Vector4d>> Normals;
	vector<Vector2d, Eigen::aligned_allocator<Vector2d>> UVs;
};

void LoadMesh(string inputfile, shared_ptr<Mesh> mesh);