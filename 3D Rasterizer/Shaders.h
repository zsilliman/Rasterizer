#pragma once
#include <Eigen/Dense>
#include <SDL.h>
#include <functional>

struct RenderableVertex {
	Eigen::Vector4d vertexPosition;
	Eigen::Vector4d NDCVertex;
	Eigen::Vector4d vertexNormal;
	Eigen::Vector2d UV;
	Eigen::Vector3d screenPosition;
	double w;
	SDL_Color color;
};

struct RenderableTriangle {
	RenderableVertex Vertices[3];
	Eigen::Vector3d screenSpaceNormal; //Normal is located in view space (x, y) correspond to screen space. z corresponds to normalized NDC.
};

struct IncrementalValues {
	float depth;
};

struct InterpolatedValues {
	Eigen::Vector3d barycentricCoord;
	RenderableVertex interpolatedValues;
	float depth;
};

struct ObjectMatrices {
	Eigen::Matrix4d ObjToWorldMatrix;
	Eigen::Matrix4d WorldToObjectMatrix;
	Eigen::Matrix4d WorldToCameraMatrix;
	Eigen::Matrix4d CameraToWorldMatrix;
	Eigen::Matrix4d CameraProjectionMatrix;
	Eigen::Matrix4d CameraToScreenMatrix;
};

typedef std::function<void(RenderableVertex&, const ObjectMatrices&)> VertexShader;

typedef std::function<SDL_Color(const InterpolatedValues&)> FragmentShader; //SDL_Color FragmentShader(const InterpolatedValues& frag);


/// <summary>
/// The vertex is provided to the player in object space
/// </summary>
/// <param name="vertex"></param>
inline void DEFAULT_VERTEX_SHADER(RenderableVertex& vertex) {

}

inline SDL_Color DEFAULT_FRAGMENT_SHADER(const InterpolatedValues& frag) {
	return frag.interpolatedValues.color;
}

inline SDL_Color BLUE_FRAGMENT_SHADER(const InterpolatedValues& v) {
	Eigen::Vector4d cam = Eigen::Vector4d(0, 0, 1, 0);
	cam << 0, 0, 1, 0;
	SDL_Color color;
	color.r = 0;
	color.g = 0;
	color.b = v.interpolatedValues.vertexNormal.dot(cam) * 255;
	return color;
}

inline SDL_Color RED_FRAGMENT_SHADER(const InterpolatedValues& v) {
	Eigen::Vector4d cam = Eigen::Vector4d(0, 0, 1, 0);
	SDL_Color color;
	color.r = v.interpolatedValues.vertexNormal.dot(cam) * 255;
	color.g = 0;
	color.b = 0;
	return color;
}