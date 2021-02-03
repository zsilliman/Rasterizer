#pragma once
#include "AssetLoader.h"
#include <map>
#include <string>
#include "SDL.h"
#include "Mesh.h"
#include "ObjLoader.h"
#include "TextureLoader.h"

class AssetManager
{

public:

	AssetManager();

	std::shared_ptr<SDL_Surface> LoadTexture(std::string filename);
	void UnloadTexture(std::string filename);

	std::shared_ptr<Mesh> LoadObj(std::string filename);
	void UnloadObj(std::string filename);

	void UnloadAll();

protected:

	//Loaded Asset Maps
	std::map<std::string, std::shared_ptr<SDL_Surface>> texture_map;
	std::map<std::string, std::shared_ptr<Mesh>> obj_map;

	//Loaders
	std::shared_ptr <ObjLoader> obj_loader;
	std::shared_ptr <TextureLoader> tex_loader;

};