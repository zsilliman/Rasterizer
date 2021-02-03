#pragma once

#include "Shaders.h"
#include <SDL.h>
#include <Eigen/Dense>

class Material
{

public:

	virtual void vert(RenderableVertex& vertex, const ObjectMatrices& matrices);

	virtual SDL_Color frag(const InterpolatedValues& pixel_values);

};
