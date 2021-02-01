#pragma once
#include <SDL.h>
#include "Scene.h"
#include "Shaders.h"

class SceneTexture
{
public:

	shared_ptr<Uint32[]> pixels;
	shared_ptr<float[]> z_buffer;
	int width, height;

	shared_ptr<SDL_Texture> texture;

	SceneTexture(shared_ptr<SDL_Renderer> renderer, int Width, int Height);

	void Clear();

	void CleanUp();

};

