#pragma once
#include <memory>
#include "Scene.h"

using namespace std;

class ZSRenderer
{

protected:

	shared_ptr<SceneTexture> sceneTexture;

public:

	ZSRenderer(shared_ptr<SceneTexture> sceneTexture);

	void RenderScene(shared_ptr<Scene> scene);

	void RenderMeshObject(shared_ptr<Camera> camera, MeshObject *scene_obj);

	void DrawScanline(const RenderableTriangle& tri, int startX, int endX, int Y, const IncrementalValues& iv, float dDepth, const FragmentShader& frag);

	void FillTopFlatTriangle(const RenderableTriangle& tri, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv, const FragmentShader& frag);

	void FillBottomFlatTriangle(const RenderableTriangle& tri, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv, const FragmentShader& frag);

	void FillTriangle(const RenderableTriangle& tri, const FragmentShader& frag);

};

