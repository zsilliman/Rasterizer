#pragma once
#include "SceneObject.h"

class Camera : public SceneObject
{

public:

	Matrix4d ProjectionMatrix;
	Matrix4d ScreenMatrix;

	double near, far;

	virtual Matrix4d GetProjectionMatrix() = 0;

	virtual Matrix4d GetScreenMatrix() = 0;

};
