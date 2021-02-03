#pragma once
#include "Material.h"

class TextureUVMaterial : Material
{

public:



	TextureUVMaterial();

	SDL_Color frag(const InterpolatedValues& pixel_values) override;

};
