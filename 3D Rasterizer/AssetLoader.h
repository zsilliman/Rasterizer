#pragma once
#include <string>
#include <memory>

template <typename T>

class AssetLoader {

public:
	//Virtual function to load an asset and fill a smart pointer.
	virtual std::shared_ptr<T> LoadAsset(std::string filepath) = 0;

	virtual void UnloadAsset(std::shared_ptr<T> asset) = 0;

};