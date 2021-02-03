#pragma once
#include "SDL.h"
#include "AssetLoader.h"

class TextureLoader : AssetLoader<SDL_Surface>
{

public:

	std::shared_ptr<SDL_Surface> LoadAsset(std::string filepath) override;

	void UnloadAsset(std::shared_ptr<SDL_Surface> asset) override;

};

