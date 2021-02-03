#include "TextureLoader.h"
#include "SDL_image.h"

std::shared_ptr<SDL_Surface> TextureLoader::LoadAsset(std::string filepath)
{
	return std::make_shared<SDL_Surface>(*IMG_Load(filepath.c_str()));
}


void TextureLoader::UnloadAsset(std::shared_ptr<SDL_Surface> asset) {
	SDL_FreeSurface(asset.get());
}