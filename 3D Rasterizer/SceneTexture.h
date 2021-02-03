#pragma once
#include <SDL.h>
#include "Scene.h"
#include "Shaders.h"
#include <shared_mutex>

class SceneTexture
{
public:

	shared_mutex mtx;
	shared_ptr<Uint32[]> pixels;
	shared_ptr<float[]> z_buffer;
	int width, height;

	shared_ptr<SDL_Texture> texture;

	SceneTexture(shared_ptr<SDL_Renderer> renderer, int Width, int Height);

	void ThreadSafeWritePixel(int index, Uint32 col);

	Uint32 ThreadSafeReadPixel(int index);

	void ThreadSafeWriteZBuffer(int index, float depth);

	float ThreadSafeReadZBuffer(int index);

	void ThreadSafeWritePixelAndDepth(int index, Uint32 col, float depth);

	void Clear();

	void CleanUp();

};

