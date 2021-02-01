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

void SceneTexture::Clear() {
	memset(pixels.get(), 255, width * height * sizeof(Uint32));
	for (int i = 0; i < width * height; i++) {
		z_buffer[i] = Z_BUFFER_INIT_VAL;
	}
}

void SceneTexture::CleanUp()
{

}
