#include "Material.h"
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
