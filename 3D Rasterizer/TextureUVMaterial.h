#pragma once
#include "Material.h"

class TextureUVMaterial : public Material
{

protected:

	std::shared_ptr<SDL_Surface> texture;

public:

	void SetTexture(std::shared_ptr<SDL_Surface> texture);

	virtual SDL_Color frag(const InterpolatedValues& pixel_values) override;

};
