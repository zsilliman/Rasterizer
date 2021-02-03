#include "AssetManager.h"

//Initialize all fields of asset manager
AssetManager::AssetManager() {
	texture_map = std::map<std::string, std::shared_ptr<SDL_Surface>>();
	obj_map = std::map<std::string, std::shared_ptr<Mesh>>();

	tex_loader = std::make_shared<TextureLoader>();
	obj_loader = std::make_shared<ObjLoader>();
}

//Load and unload textures

std::shared_ptr<SDL_Surface> AssetManager::LoadTexture(std::string filename)
{
	return tex_loader->LoadAsset(filename);
}

void AssetManager::UnloadTexture(std::string filename)
{
	if (texture_map.count(filename) == 0)
		return;
	tex_loader->UnloadAsset(texture_map[filename]);
}

//Load and unload objects

std::shared_ptr<Mesh> AssetManager::LoadObj(std::string filename)
{
	return obj_loader->LoadAsset(filename);
}

void AssetManager::UnloadObj(std::string filename)
{
	if (obj_map.count(filename) == 0)
		return;
	obj_loader->UnloadAsset(obj_map[filename]);
}

//Unload everything that is currently loaded

void AssetManager::UnloadAll()
{
	//Iterate over all textures and unload them
	std::map<std::string, std::shared_ptr<SDL_Surface>>::iterator tex_iterator;
	for (tex_iterator = texture_map.begin(); tex_iterator != texture_map.end(); tex_iterator++)
	{
		tex_loader->UnloadAsset(tex_iterator->second);
	}

	//Iterate over all meshes and unload them
	std::map<std::string, std::shared_ptr<Mesh>>::iterator obj_iterator;
	for (obj_iterator = obj_map.begin(); obj_iterator != obj_map.end(); obj_iterator++)
	{
		obj_loader->UnloadAsset(obj_iterator->second);
	}
}