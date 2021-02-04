#pragma once

#include "Shaders.h"
#include <SDL.h>
#include <Eigen/Dense>

class Material
{

public:

	virtual void vert(RenderableVertex& vertex, const ObjectMatrices& matrices);

	virtual SDL_Color frag(const InterpolatedValues& pixel_values);

protected:

#pragma region Useful Shader Functions

	Eigen::Vector4d SampleTexture(std::shared_ptr<SDL_Surface> texture, Eigen::Vector2d uv);

	SDL_Color SampleTextureSDLColor(std::shared_ptr<SDL_Surface> texture, Eigen::Vector2d uv);

#pragma endregion


};
