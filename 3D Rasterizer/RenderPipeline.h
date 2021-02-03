#pragma once

#include <vector>
#include <thread>
#include "Scene.h"
#include "Material.h"

class RenderPipeline
{
protected:
	//Rendering
	std::vector<MeshObject> stream;
	std::shared_ptr<SceneTexture> sceneTexture;
	std::shared_ptr<Camera> camera;

	//Threading
	std::thread pipeline_thread;
	bool thread_started = false;
	bool initialized = false;

public:
	/// <summary>
	/// A pipeline will render a set of triangles on its own thread.
	/// 
	/// Pipeline stages: (Triangle Enumeration already completed)
	///  1. Call Vertex Shader
	///  2. Triangle Culling
	///  3. Pixel Enumeration (Rasterization)
	///  4. Clipping & Visibility Problem
	///  5. Screen Mapping
	///  6. Fragment Shader
	///  7. Pixel Merging
	/// 
	/// </summary>
	void Initialize(std::shared_ptr<SceneTexture> sceneTexture);

	void AddMeshObject(MeshObject& mesh_object);

	void ClearMeshObjects();

	void SetCamera(std::shared_ptr<Camera> camera);

	void StartInNewThread();

	void Start();

	void StallUntilThreadExits();

	void RenderMeshObject(shared_ptr<Camera> camera, MeshObject& scene_obj);

protected:

	void TransformVertices(RenderableTriangle& tri, Material& material, ObjectMatrices& matrices);

	void EnumeratePixels(RenderableTriangle& tri, Material& material);

	void DrawScanline(const RenderableTriangle& tri, Material& material, int startX, int endX, int y, const IncrementalValues& iv, float depthDx);

	void FillTriangle(const RenderableTriangle& tri, Material& material);

	void FillBottomFlatTriangle(const RenderableTriangle& tri, Material& material, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv);

	void FillTopFlatTriangle(const RenderableTriangle& tri, Material& material, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv);

	void MergePixel(SDL_Color color, int pixel_index, float depth);

};

