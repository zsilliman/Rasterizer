#include "Material.h"
#include "ZSMath.h"
#include <cmath>

void Material::vert(RenderableVertex & vertex, const ObjectMatrices& matrices)
{
	vertex.vertexPosition = matrices.ObjToWorldMatrix * vertex.vertexPosition;
	vertex.vertexNormal = matrices.WorldToObjectMatrix.transpose() * vertex.vertexNormal;
};

SDL_Color Material::frag(const InterpolatedValues& pixel_values)
{
	Eigen::Vector4d directional_light = Eigen::Vector4d(0, 0, 1, 0);
	float light_intensity = 1;

	float L = std::max(0.0, pixel_values.interpolatedValues.vertexNormal.dot(-directional_light));

	Eigen::Vector3d color_vec;
	color_vec << 255, 0, 0;
	color_vec *= L;

	SDL_Color color;
	color.r = color_vec.x();
	color.g = color_vec.y();
	color.b = color_vec.z();
	return color;
}

//Sourced from: https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface
Uint32 getpixel(std::shared_ptr<SDL_Surface> texture, int x, int y)
{
    int bpp = texture->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8* p = (Uint8*)texture->pixels + y * texture->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16*)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32*)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

Eigen::Vector4d Material::SampleTexture(std::shared_ptr<SDL_Surface> texture, Eigen::Vector2d uv)
{
    SDL_Color rgba = SampleTextureSDLColor(texture, uv);

    Eigen::Vector4d result;
    result << rgba.r, rgba.g, rgba.b, rgba.a;
    return result;
}

SDL_Color Material::SampleTextureSDLColor(std::shared_ptr<SDL_Surface> texture, Eigen::Vector2d uv)
{
    SDL_Color rgba;
    if (texture == nullptr) {
        rgba.r = 0;
        rgba.g = 0;
        rgba.b = 0;
        rgba.a = 0;
        return rgba;
    }

    float p_x = ZSMath::NonNegativeModulo(uv.x() * texture->w, texture->w);
    float p_y = ZSMath::NonNegativeModulo(uv.y() * texture->h, texture->h);
    Uint32 col = getpixel(texture, p_x, p_y);

    SDL_GetRGBA(col, texture->format, &rgba.r, &rgba.g, &rgba.b, &rgba.a);
    return rgba;
}
