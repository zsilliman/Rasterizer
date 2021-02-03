#pragma once
#include "Mesh.h"
#include "AssetLoader.h"

class ObjLoader : AssetLoader<Mesh>
{

public:
	//Override of asset load function found in AssetLoader
	std::shared_ptr<Mesh> LoadAsset(std::string filepath) override;

	void UnloadAsset(std::shared_ptr<Mesh> asset) override;

};
