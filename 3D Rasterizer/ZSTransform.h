#pragma once

#include <Eigen/Dense>
#include <vector>

#define PI 3.14159265359
#define Deg2Rad PI / 180
#define Rad2Deg 180 / PI

using namespace Eigen;

class ZSTransform
{

protected:
	//Scale
	Vector3d scale;
	Matrix4d scaleMatrix;

	//Translation
	Vector3d localPosition;
	Matrix4d translationMatrix;

	//Rotation
	Vector3d rotation;
	Matrix4d rotationMatrix;

	//Transform
	Matrix4d transformationMatrix;
	Matrix4d inverseTransformationMatrix;

public:

	ZSTransform* parent = NULL;
	std::vector<ZSTransform> children;

	ZSTransform();

	ZSTransform(const Vector3d &rotation, const Vector3d &scale, const Vector3d &localPosition);

#pragma region Getters & Setters

	Vector3d GetScale();
	void SetScale(const Vector3d &scale);
	void SetScale(double x, double y, double z);

	Vector3d GetLocalPosition();
	void SetLocalPosition(const Vector3d &localPosition);
	void SetLocalPosition(double x, double y, double z);

	void GetWorldPosition(Vector3d &worldPosition);

	Vector3d GetRotation();
	void SetRotation(const Vector3d &rotation);
	void SetRotation(double x, double y, double z);

	Matrix4d GetWorldTransformationMatrix();
	Matrix4d GetLocalTransformationMatrix();
	Matrix4d GetInverseLocalTransformationMatrix();
	Matrix4d GetInverseWorldTransformationMatrix();

#pragma endregion

#pragma region Helpers
	
	void ApplyTransformations();

	void LookAt(const Vector3d &worldPosition);

#pragma endregion

#pragma region Static Functions

	static Matrix4d CreateScaleMatrix(double x, double y, double z);
	static Matrix4d CreateScaleMatrix(Vector3d &scale);

	static Matrix4d CreateTranslationMatrix(double x, double y, double z);
	static Matrix4d CreateTranslationMatrix(Vector3d &scale);

	static Matrix4d CreateRotationMatrix(double x, double y, double z);
	static Matrix4d CreateInverseRotationMatrix(Vector3d& eulerAngles);
	static Matrix4d CreateInverseRotationMatrix(double x, double y, double z);
	static Matrix4d CreateRotationMatrix(Vector3d &eulerAngles);

	static Matrix4d CreateTransformationMatrix(Vector3d &rotation, Vector3d &scale, Vector3d &translation);
	static Matrix4d CreateInverseTransformationMatrix(Vector3d &rotation, Vector3d &scale, Vector3d &translation);

#pragma endregion

};

