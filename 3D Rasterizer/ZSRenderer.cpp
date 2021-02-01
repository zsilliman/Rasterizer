#include "ZSRenderer.h"
#include <iostream>
#include <string>
#include "ZSMath.h"

ZSRenderer::ZSRenderer(shared_ptr<SceneTexture> sceneTexture)
{
	this->sceneTexture = sceneTexture;
}

Uint32 ColorToUint(int R, int G, int B)
{
	return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

SDL_Color UintToColor(Uint32 color)
{
	SDL_Color tempcol;
	tempcol.a = 255;
	tempcol.r = (color >> 16) & 0xFF;
	tempcol.g = (color >> 8) & 0xFF;
	tempcol.b = color & 0xFF;
	return tempcol;
}

void ZSRenderer::RenderScene(shared_ptr<Scene> scene)
{
	for (int i = 0; i < scene->meshes.size(); i++) {
		RenderMeshObject(scene->camera, &scene->meshes[i]);
	}
	int success = SDL_UpdateTexture(sceneTexture->texture.get(), NULL, sceneTexture->pixels.get(), sceneTexture->width * sizeof(Uint32));
	if (success != 0)
		printf("Failed to update texture to pixel data...");
}

void Swap(RenderableVertex& A, RenderableVertex& B) {
	RenderableVertex tmp;
	tmp.screenPosition << A.screenPosition;
	tmp.vertexPosition << A.vertexPosition;
	tmp.vertexNormal << A.vertexNormal;
	tmp.UV << A.UV;

	A.screenPosition << B.screenPosition;
	A.vertexPosition << B.vertexPosition;
	A.vertexNormal << B.vertexNormal;
	A.UV << B.UV;

	B.screenPosition << tmp.screenPosition;
	B.vertexPosition << tmp.vertexPosition;
	B.vertexNormal << tmp.vertexNormal;
	B.UV << tmp.UV;
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

//Requires that triangles are already in the view space
bool CullTriangle(RenderableTriangle& tri) {
	Vector3d view = tri.Vertices[0].vertexPosition.head<3>();
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

void ZSRenderer::RenderMeshObject(shared_ptr<Camera> camera, MeshObject* scene_obj)
{
	//Net transformation matrix M
	Eigen::Matrix4d ModelToCamera = camera->transform.GetInverseWorldTransformationMatrix() * scene_obj->transform.GetWorldTransformationMatrix();
	Eigen::Matrix4d CameraToProj = camera->GetProjectionMatrix();
	Eigen::Matrix4d ProjToScreen = camera->GetScreenMatrix();

	for (int i = 0; i < scene_obj->mesh->Shapes.size(); i++) {
		Shape s = scene_obj->mesh->Shapes[i];
		for (int j = 0; j < s.Triangles.size(); j++) {
			Triangle t = s.Triangles[j];
			RenderableTriangle tri;
			//3 vertices per triangle
			for (int k = 0; k < 3; k++) {
				MeshIndex idx = t.indices[k];
				tri.Vertices[k].vertexPosition << scene_obj->mesh->Vertices[idx.VertexIndex];
				tri.Vertices[k].vertexNormal << scene_obj->mesh->Normals[idx.NormalIndex];
				tri.Vertices[k].UV << scene_obj->mesh->UVs[idx.UVIndex];
				//Execute vertex shader
				scene_obj->vert(tri.Vertices[k]);
				//Compute Camera Space Position
				Eigen::Vector4d v_cam = ModelToCamera * tri.Vertices[k].vertexPosition;
				tri.Vertices[k].vertexNormal = ModelToCamera * tri.Vertices[k].vertexNormal;
				tri.Vertices[k].vertexPosition << v_cam;
				Eigen::Vector4d ndc = CameraToProj * v_cam;
				ndc /= ndc.w();
				tri.Vertices->NDCVertex << ndc;
				Eigen::Vector4d v_screen = ProjToScreen * ndc;
				tri.Vertices[k].screenPosition << v_screen.head<3>();
				tri.Vertices[k].color = t.Color;
			}
			//Cull Triangle
			bool cull = CullTriangle(tri);
			if (!cull) {
				ComputeScreenNormal(tri);
				//Draw Triangle
				SortVertices(tri);
				/*std::cout << "Screen0" << "\n" << tri.Vertices[0].screenPosition << "\n\n";
				std::cout << "Screen1" << "\n" << tri.Vertices[1].screenPosition << "\n\n";
				std::cout << "Screen2" << "\n" << tri.Vertices[2].screenPosition << "\n\n";
				std::cout << "V0" << "\n" << tri.Vertices[0].vertexPosition << "\n\n";
				std::cout << "V1" << "\n" << tri.Vertices[1].vertexPosition << "\n\n";
				std::cout << "V2" << "\n" << tri.Vertices[2].vertexPosition << "\n\n";*/
				FillTriangle(tri, scene_obj->frag);
			}
		}
	}
}

Vector3d ComputeBarycentricCoords(Vector2d P, Vector2d A, Vector2d B, Vector2d C) {
	Vector2d v0 = B - A, v1 = C - A, v2 = P - A;
	double d00 = v0.dot(v0);
	double d01 = v0.dot(v1);
	double d11 = v1.dot(v1);
	double d20 = v2.dot(v0);
	double d21 = v2.dot(v1);
	double denom = d00 * d11 - d01 * d01;
	double v = (d11 * d20 - d01 * d21) / denom;
	double w = (d00 * d21 - d01 * d20) / denom;
	double u = 1.0 - v - w;
	Vector3d baryCoords;
	baryCoords << v, w, u;
	return baryCoords;
}

void InterpolateValues(const RenderableTriangle& tri, const Vector3d& barycentricCoords, InterpolatedValues& v) {
	v.barycentricCoord << barycentricCoords;
	v.interpolatedValues.UV = tri.Vertices[0].UV * barycentricCoords.x() + tri.Vertices[1].UV * barycentricCoords.y() + tri.Vertices[2].UV * barycentricCoords.z();
	v.interpolatedValues.vertexNormal = tri.Vertices[0].vertexNormal * barycentricCoords.x() + tri.Vertices[1].vertexNormal * barycentricCoords.y() + tri.Vertices[2].vertexNormal * barycentricCoords.z();
	v.interpolatedValues.vertexPosition = tri.Vertices[0].vertexPosition * barycentricCoords.x() + tri.Vertices[1].vertexPosition * barycentricCoords.y() + tri.Vertices[2].vertexPosition * barycentricCoords.z();
}

//Iterates from startX to endX. Could traverse in either direction.
void ZSRenderer::DrawScanline(const RenderableTriangle& tri, int startX, int endX, int y, const IncrementalValues& iv, float depthDx, const FragmentShader& frag)
{
	//Swap to order start and end
	//if (startX > endX) {
	//	int tmp = startX;
	//	startX = endX;
	//	endX = tmp;
	//}

	//Checks if it is on screen
	if (y >= sceneTexture->height || y < 0 || (startX < 0 && endX < 0) || (startX >= sceneTexture->width && endX >= sceneTexture->width))
		return;

	int direction = ZSMath::Sign(endX - startX);

	startX = ZSMath::clampInt(startX, 0, sceneTexture->width-1);
	endX = ZSMath::clampInt(endX, 0, sceneTexture->width-1);

	//Precompute part of the index
	int offset = (sceneTexture->height - y - 1) * sceneTexture->width;
	float depth = iv.depth;
	//Iterate over pixels
	for (int x = startX; x != endX; x+=direction) {
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
		//printf("iv.depth=%f", iv.depth);
		//printf("dDepth=%f", depthDx);
		if (depth <= buff_value && depth >= -1 && depth <= 1) {
			//Call Fragment shader
			SDL_Color col = frag(v);
			//cout << "vertexNormal\n" << tri.cullNormal;
			//col.r = (depth - 0.96) / 0.4 * 200, 0.0, 255.0;
			//col.g = col.r;
			//col.b = col.r;
			sceneTexture->pixels[offset + x] = ColorToUint(col.r, col.g, col.b);
			sceneTexture->z_buffer[offset + x] = depth;
		}
		depth += depthDx * direction;
	}
}

//A.y() >= B.y() == C.y() 
void ZSRenderer::FillBottomFlatTriangle(const RenderableTriangle& tri, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv, const FragmentShader& frag)
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
	int ClampedStartY = ZSMath::clampInt(StartY, 0, sceneTexture->height-1);
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
		DrawScanline(tri, (int)leftx, (int)std::ceil(rightx), scanlineY, iv, depthDx, frag);
		//Compute depth by undoing the scanline increments, then incrementing by dy. We do this because we draw left to right. (snaking could be more efficient)
		iv.depth += depthDy;
		iv.depth -= depthDx * invslope_left;
		//Compute triangle edges by incrementing the values by dx/dy
		leftx -= invslope_left;
		rightx -= invslope_right;
	}
	DrawScanline(tri, (int)minx, std::ceil(maxx), EndY, iv, depthDx, frag);
}

//A.y() == B.y() >= C.y() 
void ZSRenderer::FillTopFlatTriangle(const RenderableTriangle& tri, const Vector3d& A, const Vector3d& B, const Vector3d& C, IncrementalValues& iv, const FragmentShader& frag)
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
	float depthDx = -tri.screenSpaceNormal.x()/tri.screenSpaceNormal.z();
	float depthDy = -tri.screenSpaceNormal.y()/tri.screenSpaceNormal.z();

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
		DrawScanline(tri, (int)leftx, (int)std::ceil(rightx), scanlineY, iv, depthDx, frag);
		iv.depth += depthDy;
		iv.depth += depthDx * invslope_left;
		//Compute triangle edges by incrementing the values by dx/dy
		leftx += invslope_left;
		rightx += invslope_right;
	}
	DrawScanline(tri, (int)minx, std::ceil(maxx), EndY, iv, depthDx, frag);
}

//Assumes the vertices are in the proper order before calling. Call "Sort Vertices" first.
//Vertices must be sorted in descending order by the y coordinate in screen space.
void ZSRenderer::FillTriangle(const RenderableTriangle& tri, const FragmentShader& frag)
{
	Vector3d A, B, C, D;
	A << tri.Vertices[0].screenPosition.head<3>();
	B << tri.Vertices[1].screenPosition.head<3>();
	C << tri.Vertices[2].screenPosition.head<3>();

	IncrementalValues iv;
	//Handle cases if triangle is flat top or flat bottom
	if ((int)A.y() == (int)B.y()) {
		FillTopFlatTriangle(tri, A, B, C, iv, frag);
		return;
	}
	else if ((int)B.y() == (int)C.y()) {
		FillBottomFlatTriangle(tri, A, B, C, iv, frag);
		return;
	}

	//Compute D for generic case
	double Dx = C.x() + (A.x() - C.x()) * ((B.y() - C.y()) / (A.y() - C.y()));
	double Dy = B.y();
	D << Dx, Dy, 0;//No need to populate z value here because this vertex is never directly referenced

	//Fill Top and Bottom Triangles
	FillBottomFlatTriangle(tri, A, B, D, iv, frag);
	FillTopFlatTriangle(tri, B, D, C, iv, frag);
}