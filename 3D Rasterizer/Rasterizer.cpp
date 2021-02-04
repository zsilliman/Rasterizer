#include "Rasterizer.h"
#include "ZSWindow.h"
#include "SceneTexture.h"
#include "stdio.h"
#include <memory>
#include "Scene.h"
#include <SDL.h>
#include "ZSRenderer.h"
#include "Shaders.h"
#include "AssetManager.h"
#include "TextureUVMaterial.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 400

using namespace std;

shared_ptr<ZSWindow> window;
shared_ptr<AssetManager> asset_manager;

void CreateScene(shared_ptr<Scene> scene) {
	
	//Load Assets
	std::shared_ptr<Mesh> cube_mesh = asset_manager->LoadObj("Assets/Testing/cubemesh.obj");
	std::shared_ptr<Mesh> quad_mesh = asset_manager->LoadObj("Assets/Testing/quadmesh.obj");
	std::shared_ptr<SDL_Surface> checkered = asset_manager->LoadTexture("Assets/Testing/checkerboard.bmp");
	if (checkered == nullptr)
		printf("Checkered texture is null...");

	//Create Materials
	shared_ptr<Material> default_material = make_shared<Material>();
	shared_ptr<TextureUVMaterial> checkered_diffuse = make_shared<TextureUVMaterial>();
	checkered_diffuse->SetTexture(checkered);

	scene->meshes = vector<MeshObject>();
	scene->camera = make_shared<PerspectiveCamera>();
	scene->camera->SetProjectionMatrix(90, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0, 25.0);
	scene->camera->transform.SetLocalPosition(0, 0, -6);
	scene->camera->transform.SetScale(1, 1, 1);
	scene->camera->transform.SetRotation(0, 0, 0);
	scene->camera->transform.ApplyTransformations();

	MeshObject mesh = MeshObject(cube_mesh);
	mesh.transform.SetLocalPosition(0, 0, 0);
	mesh.transform.SetScale(1, 1, 1);
	mesh.transform.SetRotation(0, 5, 0);
	mesh.transform.ApplyTransformations();
	mesh.transform.parent = NULL;
	mesh.materials.push_back(checkered_diffuse);
	scene->meshes.push_back(mesh);

	MeshObject quad = MeshObject(quad_mesh);
	quad.transform.SetLocalPosition(0, 0, 0);
	quad.transform.SetScale(1.4, 1.4, 1.4);
	quad.transform.SetRotation(0, 180, 0);
	quad.transform.ApplyTransformations();
	quad.transform.parent = NULL;
	quad.materials.push_back(checkered_diffuse);
	//scene->meshes.push_back(quad);
}

int main(int argc, char* argv[])
{
	//Initialize random seed
	srand(time(NULL));

	//Setup Asset Manager
	asset_manager = make_shared<AssetManager>();

	// Body of the program goes here.
	//Start up SDL and create window
	window = make_shared<ZSWindow>("Zach's 3D Rasterizer", SCREEN_WIDTH, SCREEN_HEIGHT);
	bool initialized = window->init();
	if (!initialized)
	{
		printf("Failed to initialize!\n");
	}

	shared_ptr<Scene> scene = make_shared<Scene>();
	CreateScene(scene);

	shared_ptr<SceneTexture> scene_tex = make_shared<SceneTexture>(window->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	shared_ptr<ZSRenderer> zs_renderer = make_shared<ZSRenderer>(scene_tex);
	
	float rotationX = 0;
	float rotationY = 5;
	float Y = 0;
	//SDL_Delay(30000);
	int frame_count = 0;
	time_t seconds;

	int high = 0;
	int low = 10000;
	seconds = time(NULL);
	while (true) {
		zs_renderer->RenderScene(scene);
		scene->meshes[0].transform.SetRotation(rotationX, rotationY, rotationX);
		scene->meshes[0].transform.ApplyTransformations();
		//scene->meshes[1].transform.SetRotation(0, rotationY, 0);
		//scene->meshes[1].transform.ApplyTransformations();
		rotationX += 10;
		rotationY += 10;

		window->ClearWindow();
		window->RenderSceneTexture(scene_tex);
		window->Present();
		scene_tex->Clear();
		//printf("done rendering frame %i...\n", frame_count);
		frame_count++;
		if (time(NULL) - seconds >= 1) {
			if (frame_count > high) {
				high = frame_count;
			}
			if (frame_count < low) {
				low = frame_count;
			}
			std::cout << "Frame count this second:" << frame_count << "   HIGH=" << high << "   LOW=" << low <<"\n";
			frame_count = 0;
			seconds = time(NULL);
		}
		//SDL_Delay(100);
	}

	//Free resources and close SDL
	asset_manager->UnloadAll();
	window->close();

	return 0;
}