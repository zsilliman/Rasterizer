#include "ZSRenderer.h"
#include <iostream>
#include <string>
#include "ZSMath.h"
#include "RenderPipeline.h"

ZSRenderer::ZSRenderer(shared_ptr<SceneTexture> sceneTexture)
{
	this->sceneTexture = sceneTexture;
	for (int i = 0; i < NUMTHREADS; i++) {
		pipelines[i].Initialize(sceneTexture);
	}
}

void ZSRenderer::RenderScene(shared_ptr<Scene> scene)
{
	for (int i = 0; i < scene->meshes.size(); i++) {
		pipelines[0].RenderMeshObject(scene->camera, scene->meshes[i]);
	}
	int success = SDL_UpdateTexture(sceneTexture->texture.get(), NULL, sceneTexture->pixels.get(), sceneTexture->width * sizeof(Uint32));
	if (success != 0)
		printf("Failed to update texture to pixel data...");
}

void ZSRenderer::RenderSceneMultithreaded(shared_ptr<Scene> scene)
{
	for (int i = 0; i < NUMTHREADS; i++) {
		pipelines[i].SetCamera(scene->camera);
		pipelines[i].ClearMeshObjects();
	}

	int thread_index = 0;
	for (int i = 0; i < scene->meshes.size(); i++) {

		pipelines[thread_index].AddMeshObject(scene->meshes[i]);

		thread_index = (thread_index + 1) % NUMTHREADS;
	}

	for (int i = 0; i < NUMTHREADS; i++) {
		pipelines[i].Start();
	}
	for (int i = 0; i < NUMTHREADS; i++) {
		pipelines[i].StallUntilThreadExits();
	}

	int success = SDL_UpdateTexture(sceneTexture->texture.get(), NULL, sceneTexture->pixels.get(), sceneTexture->width * sizeof(Uint32));
	if (success != 0)
		printf("Failed to update texture to pixel data...");
}