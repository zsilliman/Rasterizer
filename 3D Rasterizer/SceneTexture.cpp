#include "SceneTexture.h"
#include <iostream>
#include <math.h>
#include "ZSMath.h"
#include <limits>

#define Z_BUFFER_INIT_VAL 9999.0

SceneTexture::SceneTexture(shared_ptr<SDL_Renderer> renderer, int Width, int Height)
{
	width = Width;
	height = Height;
	texture = shared_ptr<SDL_Texture>(SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, Width, Height),
										[](SDL_Texture* tex) {SDL_DestroyTexture(tex); });

	//Setup pixels
	pixels = shared_ptr<Uint32[]>(new Uint32[width * height]);
	memset(pixels.get(), 255, width * height * sizeof(Uint32));

	//Setup z-buffer
	z_buffer = shared_ptr<float[]>(new float[width * height]);
	for (int i = 0; i < width * height; i++) {
		z_buffer[i] = Z_BUFFER_INIT_VAL;
	}
}

void SceneTexture::ThreadSafeWritePixel(int index, Uint32 col) {
	std::lock_guard<std::shared_mutex> lock(mtx);
	pixels[index] = col;
}

Uint32 SceneTexture::ThreadSafeReadPixel(int index) {
	std::shared_lock<std::shared_mutex> lock(mtx);
	Uint32 col = pixels[index];
	return col;
}

void SceneTexture::ThreadSafeWriteZBuffer(int index, float depth) {
	std::lock_guard<std::shared_mutex> lock(mtx);
	z_buffer[index] = depth;
}

float SceneTexture::ThreadSafeReadZBuffer(int index) {
	std::shared_lock<std::shared_mutex> lock(mtx);
	float z = z_buffer[index];
	return z;
}

void SceneTexture::ThreadSafeWritePixelAndDepth(int index, Uint32 col, float depth)
{
	std::lock_guard<std::shared_mutex> lock(mtx);
	pixels[index] = col;
	z_buffer[index] = depth;
}

void SceneTexture::Clear() {
	memset(pixels.get(), 255, width * height * sizeof(Uint32));
	for (int i = 0; i < width * height; i++) {
		z_buffer[i] = Z_BUFFER_INIT_VAL;
	}
}

void SceneTexture::CleanUp()
{

}
