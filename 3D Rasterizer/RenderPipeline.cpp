#include "RenderPipeline.h"
#include "ZSMath.h"

#include <thread>
#include <memory>
#include <cassert>

void RenderPipeline::Initialize(std::shared_ptr<SceneTexture> sceneTexture) {
	this->sceneTexture = sceneTexture;
	this->initialized = true;
}

void RenderPipeline::SetCamera(std::shared_ptr<Camera> camera) {
	this->camera = camera;
}

void RenderPipeline::AddMeshObject(MeshObject& mesh_object)
{
	this->stream.push_back(mesh_object);
}

void RenderPipeline::ClearMeshObjects()
{
	this->stream.clear();
}

#pragma region Threading

void RenderPipeline::StartInNewThread()
{
	this->pipeline_thread = std::thread(&RenderPipeline::Start, this);
	thread_started = true;
}


void RenderPipeline::Start()
{
	if (!initialized) {
		printf("Render Pipeline was not initialized before start.");
		return;
	}
	for (MeshObject obj : stream) {
		RenderMeshObject(camera, obj);
	}
}

void RenderPipeline::StallUntilThreadExits()
{
	if (thread_started) {
		pipeline_thread.join();
	}
}

#pragma endregion

#pragma region Merge Pixels

Uint32 ColorToUint(int R, int G, int B)
{
	return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

void RenderPipeline::MergePixel(SDL_Color color, int pixel_index, float depth)
{
	Uint32 col = ColorToUint(color.r, color.g, color.b);
	sceneTexture->ThreadSafeWritePixelAndDepth(pixel_index, col, depth);
}

#pragma endregion

#pragma region Rasterization

void Swap(RenderableVertex& A, RenderableVertex& B) {
	RenderableVertex tmp;
	tmp = A;
	A = B;
	B = tmp;
}

void SortVertices(RenderableTriangle& triangle) {
	if (triangle.Vertices[0].screenPosition.y() < triangle.Vertices[1].screenPosition.y()) {
		Swap(triangle.Vertices[0], triangle.Vertices[1]);
	}
	if (triangle.Vertices[1].screenPosition.y() < triangle.Vertices[2].screenPosition.y()) {
		Swap(triangle.Vertices[1], triangle.Vertices[2]);
	}
	if (triangle.Vertices[0].screenPosition.y() < triangle.Vertices[1].screenPosition.y()) {
		Swap(triangle.Vertices[0], triangle.Vertices[1]);
	}
}

void RenderPipeline::EnumeratePixels(RenderableTriangle& tri, Material& material)
{
	SortVertices(tri);
	FillTriangle(tri, material);
}

//Iterates from startX to endX. Could traverse in either direction.
void RenderPipeline::DrawScanline(const RenderableTriangle& tri, Material& material, int startX, int endX, int y, const IncrementalValues& iv, float depthDx)
{
	//Checks if it is on screen
	if (y >= sceneTexture->height || y < 0 || (startX < 0 && endX < 0) || (startX >= sceneTexture->width && endX >= sceneTexture->width))
		return;

	int direction = ZSMath::Sign(endX - startX);

	startX = ZSMath::clampInt(startX, 0, sceneTexture->width - 1);
	endX = ZSMath::clampInt(endX, 0, sceneTexture->width - 1);

	//Precompute part of the index
	int offset = (sceneTexture->height - y - 1) * sceneTexture->width;
	float depth = iv.depth;
	//Iterate over pixels
	for (int x = startX; x != endX; x += direction) {
		Vector2d P;
		P << x, y;

		//INTERPOLATE VALUES
		//Vector3d baryCoords = ComputeBarycentricCoords(P, tri.Vertices[0].screenPosition.head<2>(), tri.Vertices[1].screenPosition.head<2>(), tri.Vertices[2].screenPosition.head<2>());
		InterpolatedValues v;
		v.interpolatedValues.color = tri.Vertices[0].color;
		v.interpolatedValues.vertexNormal = tri.Vertices[0].vertexNormal;
		//v.interpolatedValues.screenPosition << x, y, 0;
		//InterpolateValues(tri, baryCoords, v);

		//Handle Z-Buffer
		float buff_value = sceneTexture->z_buffer[offset + x];
		if (depth < buff_value && depth >= -1 && depth <= 1) {
			//Call Fragment shader
			SDL_Color col = material.frag(v);
			//MergePixel(col, offset + x, depth);
			int index = offset + x;
			//Uint32 color = ColorToUint(col.r, col.g, col.b);
			MergePixel(col, index, depth);
		}
		depth += depthDx * direction;
	}
}

//A.y() >= B.y() == C.y() 
void RenderPipeline::FillBottomFlatTriangle(const RenderableTriangle& tri, Material& material, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv)
{
	double invslope_left;
	double invslope_right;
	double minx = B.x();
	double maxx = C.x();

	if (B.x() < C.x()) {
		invslope_left = (B.x() - A.x()) / (B.y() - A.y());
		invslope_right = (C.x() - A.x()) / (C.y() - A.y());
	}
	else {
		minx = C.x();
		maxx = B.x();
		invslope_left = (C.x() - A.x()) / (C.y() - A.y());
		invslope_right = (B.x() - A.x()) / (B.y() - A.y());
	}

	double StartY = (int)(A.y() - 0.5) + 0.5;
	//If the starting location is below the screen, then the whole triangle is below the screen.
	if (StartY < 0)
		return;

	double leftx = A.x();
	double rightx = A.x();
	//increment slightly
	leftx -= (A.y() - StartY) * invslope_left;
	rightx -= (A.y() - StartY) * invslope_right;

	iv.depth = tri.Vertices[0].screenPosition.z();
	float depthDx = -tri.screenSpaceNormal.x() / tri.screenSpaceNormal.z();
	float depthDy = tri.screenSpaceNormal.y() / tri.screenSpaceNormal.z(); //We decrement y so multiple this by negative 1.
	//cout << "CullX=" << tri.cullNormal.x() << "  CullY=" << tri.cullNormal.y() << "  CullZ=" << tri.cullNormal.z()<< "\n";
	//cout << "DepthDX=" << depthDy << "  DepthDY=" << depthDy << "\n";
	//Get starting Y location (top of  triangle)

	//Clamp the starting location to fit within [0, HEIGHT-1]
	int ClampedStartY = ZSMath::clampInt(StartY, 0, sceneTexture->height - 1);
	int diff = (int)StartY - ClampedStartY;
	//Any amount moved by the clamping needs to be accounted for in the current x variables [curx1] and [curx2]
	leftx -= diff * invslope_left;
	rightx -= diff * invslope_right;
	StartY = ClampedStartY;

	//Get ending Y location. This will be a smaller value than starting Y
	int EndY = (int)std::round(B.y());
	//Exit if the end point (lowest point) is above the screen.
	if (EndY >= sceneTexture->height)
		return;
	//Stop rendering if it is below the screen
	if (EndY < 0)
		EndY = 0;

	//printf("Fill Bottom Flat Triangle startY=%i endY=%i\n", StartY, EndY);
	for (int scanlineY = StartY; scanlineY > EndY; scanlineY--)
	{
		DrawScanline(tri, material, (int)leftx, (int)std::ceil(rightx), scanlineY, iv, depthDx);
		//Compute depth by undoing the scanline increments, then incrementing by dy. We do this because we draw left to right. (snaking could be more efficient)
		iv.depth += depthDy;
		iv.depth -= depthDx * invslope_left;
		//Compute triangle edges by incrementing the values by dx/dy
		leftx -= invslope_left;
		rightx -= invslope_right;
	}
	DrawScanline(tri, material, (int)minx, std::ceil(maxx), EndY, iv, depthDx);
}

//A.y() == B.y() >= C.y() 
void RenderPipeline::FillTopFlatTriangle(const RenderableTriangle& tri, Material& material, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv)
{
	double invslope_left;
	double invslope_right;
	double minx = A.x();
	double maxx = B.x();

	if (A.x() < B.x()) {
		invslope_left = (A.x() - C.x()) / (A.y() - C.y());
		invslope_right = (B.x() - C.x()) / (B.y() - C.y());
	}
	else {
		minx = B.x();
		maxx = A.x();
		invslope_left = (B.x() - C.x()) / (B.y() - C.y());
		invslope_right = (A.x() - C.x()) / (A.y() - C.y());
	}

	//Get starting Y location (bottom of  triangle)
	double StartY = std::ceil(C.y() - 0.5) + 0.5;
	//If the starting location is above the screen, then the whole triangle is above the screen.
	if (StartY >= sceneTexture->height)
		return;

	//Align with first pixel center above coordinate
	double leftx = C.x();
	double rightx = C.x();
	//increment slightly
	leftx += (StartY - C.y()) * invslope_left;
	rightx += (StartY - C.y()) * invslope_right;
	iv.depth = tri.Vertices[2].screenPosition.z();
	float depthDx = -tri.screenSpaceNormal.x() / tri.screenSpaceNormal.z();
	float depthDy = -tri.screenSpaceNormal.y() / tri.screenSpaceNormal.z();

	//Clamp starting location to fit within [0, HEIGHT-1]
	int ClampedStartY = ZSMath::clampInt(StartY, 0, sceneTexture->height - 1);
	//Clamped value will always be larger than StartY.
	int diff = ClampedStartY - (int)StartY;
	//Any amount moved by the clamping needs to be accounted for in the current x variables [curx1] and [curx2]
	leftx += diff * invslope_left;
	rightx += diff * invslope_right;
	StartY = ClampedStartY;

	//Get ending Y location. This will be a larger value than starting Y
	int EndY = (int)std::round(B.y());
	//Exit if the end point (highest point) is below the screen.
	if (EndY < 0)
		return;
	//Stop rendering if it is above the screen
	if (EndY >= sceneTexture->height)
		EndY = sceneTexture->height - 1;

	//printf("Fill Top Flat Triangle startY=%i endY=%i\n", StartY, EndY);
	for (int scanlineY = StartY; scanlineY < EndY; scanlineY++)
	{
		DrawScanline(tri, material, (int)leftx, (int)std::ceil(rightx), scanlineY, iv, depthDx);
		iv.depth += depthDy;
		iv.depth += depthDx * invslope_left;
		//Compute triangle edges by incrementing the values by dx/dy
		leftx += invslope_left;
		rightx += invslope_right;
	}
	DrawScanline(tri, material, (int)minx, std::ceil(maxx), EndY, iv, depthDx);
}

//Assumes the vertices are in the proper order before calling. Call "Sort Vertices" first.
//Vertices must be sorted in descending order by the y coordinate in screen space.
void RenderPipeline::FillTriangle(const RenderableTriangle& tri, Material& material)
{
	Vector3d A, B, C, D;
	A << tri.Vertices[0].screenPosition.head<3>();
	B << tri.Vertices[1].screenPosition.head<3>();
	C << tri.Vertices[2].screenPosition.head<3>();

	IncrementalValues iv;
	//Handle cases if triangle is flat top or flat bottom
	if ((int)A.y() == (int)B.y()) {
		FillTopFlatTriangle(tri, material, A, B, C, iv);
		return;
	}
	else if ((int)B.y() == (int)C.y()) {
		FillBottomFlatTriangle(tri, material, A, B, C, iv);
		return;
	}

	//Compute D for generic case
	double Dx = C.x() + (A.x() - C.x()) * ((B.y() - C.y()) / (A.y() - C.y()));
	double Dy = B.y();
	D << Dx, Dy, 0;//No need to populate z value here because this vertex is never directly referenced

	//Fill Top and Bottom Triangles
	FillBottomFlatTriangle(tri, material, A, B, D, iv);
	FillTopFlatTriangle(tri, material, B, D, C, iv);
}


#pragma endregion

#pragma region Triangles and Vertices

void RenderPipeline::TransformVertices(RenderableTriangle& tri, Material& material, ObjectMatrices& matrices)
{
	//Iterate over vertices
	for (int i = 0; i < 3; i++) {
		//Call Vertex Shader. Assume positions are in world space after shader call.
		RenderableVertex vertex = tri.Vertices[i];
		material.vert(vertex, matrices);
		tri.Vertices[i] = vertex;

		//Compute screenspace coordinates
		Eigen::Vector4d v_cam = matrices.WorldToCameraMatrix * tri.Vertices[i].vertexPosition;
		tri.Vertices[i].vertexNormal = matrices.WorldToCameraMatrix * tri.Vertices[i].vertexNormal;
		tri.Vertices[i].vertexPosition << v_cam;
		Eigen::Vector4d ndc = matrices.CameraProjectionMatrix * v_cam;
		ndc /= ndc.w();
		tri.Vertices->NDCVertex << ndc;
		Eigen::Vector4d v_screen = matrices.CameraToScreenMatrix * ndc;
		tri.Vertices[i].screenPosition << v_screen.head<3>();
	}
}

bool CullTriangle(RenderableTriangle& tri, Vector3d camera_position) {
	Vector3d view = tri.Vertices[0].vertexPosition.head<3>() - camera_position;
	Vector3d A = tri.Vertices[1].vertexPosition.head<3>() - tri.Vertices[0].vertexPosition.head<3>();
	Vector3d B = tri.Vertices[2].vertexPosition.head<3>() - tri.Vertices[0].vertexPosition.head<3>();
	Vector3d normal = A.cross(B);
	//return false;
	return view.dot(normal) >= 0;
}

void ComputeScreenNormal(RenderableTriangle& tri) {
	Vector3d A = tri.Vertices[1].screenPosition.head<3>() - tri.Vertices[0].screenPosition.head<3>();
	Vector3d B = tri.Vertices[2].screenPosition.head<3>() - tri.Vertices[0].screenPosition.head<3>();
	tri.screenSpaceNormal = A.cross(B).normalized();
}

#pragma endregion

#pragma region Scene Objects

/// <summary>
/// This function renders a mesh object to the screen through the lens of the given camera.
/// 
/// Pipeline: Triangle Enumeration --> 
/// 
/// </summary>
/// <param name="camera"></param>
/// <param name="scene_obj"></param>
void RenderPipeline::RenderMeshObject(shared_ptr<Camera> camera, MeshObject& scene_obj)
{
	//Get Object Transform Matrices
	ObjectMatrices matrices;
	matrices.ObjToWorldMatrix = scene_obj.transform.GetWorldTransformationMatrix();
	matrices.WorldToObjectMatrix = scene_obj.transform.GetInverseWorldTransformationMatrix();
	matrices.WorldToCameraMatrix = camera->transform.GetInverseWorldTransformationMatrix();
	matrices.CameraToWorldMatrix = camera->transform.GetWorldTransformationMatrix();
	matrices.CameraProjectionMatrix = camera->GetProjectionMatrix();
	matrices.CameraToScreenMatrix = camera->GetScreenMatrix();

	Vector3d camera_position;
	camera->transform.GetWorldPosition(camera_position);

	//Triangle Enumeration
	for (int i = 0; i < scene_obj.mesh->Shapes.size(); i++) {
		Shape s = scene_obj.mesh->Shapes[i];
		for (int j = 0; j < s.Triangles.size(); j++) {
			Triangle t = s.Triangles[j];
			RenderableTriangle tri;
			//3 vertices per triangle
			for (int k = 0; k < 3; k++) {
				MeshIndex idx = t.indices[k];
				tri.Vertices[k].vertexPosition << scene_obj.mesh->Vertices[idx.VertexIndex];
				tri.Vertices[k].vertexNormal << scene_obj.mesh->Normals[idx.NormalIndex];
				tri.Vertices[k].UV << scene_obj.mesh->UVs[idx.UVIndex];
				tri.Vertices[k].color = t.Color;
			}

			Material material = scene_obj.materials[t.material_id];
			//Execute vertex shader. Assume coordinates are in world space after execution.
			TransformVertices(tri, material, matrices);

			//Cull Triangle
			bool cull = CullTriangle(tri, camera_position);
			if (!cull) {
				//Compute screen normal for use in depth buffer calculation
				ComputeScreenNormal(tri);
				//Sort vertices for drawing
				SortVertices(tri);
				//Draw triangles
				EnumeratePixels(tri, material);
			}
		}
	}
}
#pragma endregion
