#pragma once
#include <memory>
#include "Scene.h"
#include "RenderPipeline.h"

using namespace std;


class ZSRenderer
{

protected:

	static const int NUMTHREADS = 2;

	shared_ptr<SceneTexture> sceneTexture;

	RenderPipeline pipelines[NUMTHREADS];

public:

	ZSRenderer(shared_ptr<SceneTexture> sceneTexture);

	void RenderSceneMultithreaded(shared_ptr<Scene> scene);

	void RenderScene(shared_ptr<Scene> scene);

};

