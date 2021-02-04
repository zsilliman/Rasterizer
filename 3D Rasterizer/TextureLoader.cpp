#include "TextureLoader.h"
#include "SDL_image.h"

std::shared_ptr<SDL_Surface> TextureLoader::LoadAsset(std::string filepath)
{
	//std::shared_ptr<SDL_Surface> surf(IMG_Load(filepath.c_str()), SDL_FreeSurface);
	std::shared_ptr<SDL_Surface> surf(SDL_LoadBMP(filepath.c_str()), SDL_FreeSurface);
	return surf;
}


void TextureLoader::UnloadAsset(std::shared_ptr<SDL_Surface> asset) {
	//SDL_FreeSurface(asset.get());
}