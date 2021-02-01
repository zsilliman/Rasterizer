#pragma once
#include "Camera.h"

class PerspectiveCamera : public Camera
{

public:

	/// <summary>
	/// 
	/// </summary>
	/// <param name="fov">Field of View in degrees</param>
	/// <param name="screenWidth"></param>
	/// <param name="screenHeight"></param>
	/// <param name=""></param>
	/// <param name=""></param>
	void SetProjectionMatrix(double fov, double screenWidth, double screenHeight, double near, double far);

	Matrix4d GetProjectionMatrix();

	Matrix4d GetScreenMatrix();

};