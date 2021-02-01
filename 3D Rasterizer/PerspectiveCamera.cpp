#include "PerspectiveCamera.h"
#include <Eigen/Dense>
#include <math.h>
#include "ZSTransform.h"
#include <iostream>

using namespace Eigen;

void PerspectiveCamera::SetProjectionMatrix(double fov, double screenWidth, double screenHeight, double near, double far) {
	double aspectRatio = screenWidth / screenHeight;
	fov *= Deg2Rad;
	double tan_fov = tan(0.5 * fov);

	//double right = near * sin(fov * Deg2Rad / 2.0);
	//double left = -right;
	//double top = right * aspectRatio;
	//double bottom = -top;
	this->near = near;
	this->far = far;

	ProjectionMatrix << 1/(aspectRatio * tan_fov), 0, 0, 0,
						0, 1/tan_fov, 0, 0,
						0, 0, (-near-far) / (near - far), (2 * far*near) / (near - far),
						0, 0, 1, 0;

	ScreenMatrix << screenWidth/2, 0, 0, screenWidth / 2,
					0, screenHeight/2, 0, screenHeight / 2,
					0, 0, 1, 0,
					0, 0, 0, 1;
}

Matrix4d PerspectiveCamera::GetProjectionMatrix() {
	return Matrix4d(ProjectionMatrix);
}

Matrix4d PerspectiveCamera::GetScreenMatrix()
{
	return Matrix4d(ScreenMatrix);
}
