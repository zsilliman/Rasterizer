#include "TextureUVMaterial.h"
#include "ZSMath.h"

void TextureUVMaterial::SetTexture(std::shared_ptr<SDL_Surface> texture)
{
	this->texture = texture;
}

SDL_Color TextureUVMaterial::frag(const InterpolatedValues& pixel_values)
{
	//SDL_Color color;
	//color.r = 255.0 * pixel_values.interpolatedValues.UV.x();
	//color.g = 0;
	//color.b = 0;
	//color.a = 255;
	return SampleTextureSDLColor(texture, pixel_values.interpolatedValues.UV);
}
