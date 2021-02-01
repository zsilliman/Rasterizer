#include "ZSTransform.h"
#include "math.h"

ZSTransform::ZSTransform()
{
	SetRotation(Vector3d::Zero());
	SetScale(Vector3d::Ones());
	SetLocalPosition(Vector3d::Zero());
	ApplyTransformations();
}

ZSTransform::ZSTransform(const Vector3d& rotation, const Vector3d& scale, const Vector3d& localPosition)
{
	SetRotation(rotation);
	SetScale(scale);
	SetLocalPosition(localPosition);
	ApplyTransformations();
}

Vector3d ZSTransform::GetScale()
{
	Vector3d outVec;
	outVec << this->scale;
	return outVec;
}

void ZSTransform::SetScale(double x, double y, double z)
{
	this->scale << x, y, z;
	this->scaleMatrix = CreateScaleMatrix(x, y, z);
}

void ZSTransform::SetScale(const Vector3d &scale)
{
	SetScale(scale.x(), scale.y(), scale.z());
}

Vector3d ZSTransform::GetLocalPosition()
{
	Vector3d outVec;
	outVec << this->localPosition;
	return outVec;
}

void ZSTransform::SetLocalPosition(double x, double y, double z)
{
	this->localPosition << x, y, z;
	this->translationMatrix = CreateTranslationMatrix(x, y, z);
}

void ZSTransform::SetLocalPosition(const Vector3d & localPosition)
{
	SetLocalPosition(localPosition.x(), localPosition.y(), localPosition.z());
}

void ZSTransform::GetWorldPosition(Vector3d & worldPosition)
{
	if (parent != NULL) {
		Vector3d parentPosition;
		parent->GetWorldPosition(parentPosition);
		worldPosition << (parentPosition + localPosition);
	}
	else {
		worldPosition << localPosition;
	}
}

Vector3d ZSTransform::GetRotation()
{
	Vector3d outVec;
	outVec << this->rotation;
	return outVec;
}

void ZSTransform::SetRotation(double x, double y, double z)
{
	this->rotation << x, y, z;
	this->rotationMatrix = CreateRotationMatrix(x, y, z);
}

Matrix4d ZSTransform::GetWorldTransformationMatrix()
{
	Matrix4d matrix;
	matrix.setIdentity();
	if (parent != NULL) {
		Matrix4d parentWorldTransform = parent->GetWorldTransformationMatrix();
		matrix *= parentWorldTransform * transformationMatrix;
		return matrix;
	}
	else {
		return GetLocalTransformationMatrix();
	}
}

Matrix4d ZSTransform::GetLocalTransformationMatrix()
{
	Matrix4d matrix;
	matrix << transformationMatrix;
	return matrix;
}

Matrix4d ZSTransform::GetInverseLocalTransformationMatrix()
{
	Matrix4d matrix;
	matrix << inverseTransformationMatrix;
	return matrix;
}

Matrix4d ZSTransform::GetInverseWorldTransformationMatrix()
{
	Matrix4d matrix;
	matrix.setIdentity();
	if (parent != NULL) {
		Matrix4d parentWorldTransform = parent->GetInverseWorldTransformationMatrix();
		//Right multiply by the parent world transform. It will cancel out with the other transforms in the transformation hierarchy
		matrix *= inverseTransformationMatrix * parentWorldTransform;
		return matrix;
	}
	else {
		return GetInverseLocalTransformationMatrix();
	}
}

void ZSTransform::SetRotation(const Vector3d & rotation)
{
	SetRotation(rotation.x(), rotation.y(), rotation.z());
}

void ZSTransform::ApplyTransformations() {
	transformationMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	inverseTransformationMatrix = CreateInverseTransformationMatrix(rotation, scale, localPosition);
}

void ZSTransform::LookAt(const Vector3d &targetWorldPosition)
{
	Vector3d thisWorldPosition;
	GetWorldPosition(thisWorldPosition);
	Vector3d diff = targetWorldPosition - thisWorldPosition;
	double theta_x = atan2(diff.y(), diff.z());
	double theta_y = atan2(diff.x(), diff.z());
	double theta_z = atan2(diff.y(), diff.x());
	SetRotation(theta_x, theta_y, theta_z);
}

Matrix4d ZSTransform::CreateScaleMatrix(double x, double y, double z)
{
	Matrix4d S;
	S << x, 0, 0, 0,
		 0, y, 0, 0,
		 0, 0, z, 0,
		 0, 0, 0, 1;
	return S;
}

Matrix4d ZSTransform::CreateScaleMatrix(Vector3d &scale)
{
	return CreateScaleMatrix(scale.x(), scale.y(), scale.z());
}

Matrix4d ZSTransform::CreateTranslationMatrix(double x, double y, double z)
{
	Matrix4d T;
	T << 1, 0, 0, x,
		 0, 1, 0, y,
		 0, 0, 1, z,
		 0, 0, 0, 1;
	return T;
}

Matrix4d ZSTransform::CreateTranslationMatrix(Vector3d &translation)
{
	return CreateTranslationMatrix(translation.x(), translation.y(), translation.z());
}

Matrix4d ZSTransform::CreateRotationMatrix(double x, double y, double z)
{
	x *= Deg2Rad;
	y *= Deg2Rad;
	z *= Deg2Rad;
	Matrix4d Rx;
	Rx << 1, 0, 0, 0,
		  0, cos(x), -sin(x), 0,
		  0, sin(x), cos(x), 0,
		  0, 0, 0, 1;

	Matrix4d Ry;
	Ry << cos(y), 0, sin(y), 0,
		  0, 1, 0, 0,
		  -sin(y), 0, cos(y), 0,
		  0, 0, 0, 1;

	Matrix4d Rz;
	Rz << cos(z), -sin(z), 0, 0,
		  sin(z), cos(z), 0, 0,
		  0, 0, 1, 0,
		  0, 0, 0, 1;
	return Rx * Ry * Rz;
}

Matrix4d ZSTransform::CreateInverseRotationMatrix(Vector3d &eulerAngles)
{
	return CreateInverseRotationMatrix(eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
}

Matrix4d ZSTransform::CreateInverseRotationMatrix(double x, double y, double z)
{
	x *= -Deg2Rad;
	y *= -Deg2Rad;
	z *= -Deg2Rad;
	Matrix4d Rx;
	Rx << 1, 0, 0, 0,
		0, cos(x), -sin(x), 0,
		0, sin(x), cos(x), 0,
		0, 0, 0, 1;

	Matrix4d Ry;
	Ry << cos(y), 0, sin(y), 0,
		0, 1, 0, 0,
		-sin(y), 0, cos(y), 0,
		0, 0, 0, 1;

	Matrix4d Rz;
	Rz << cos(z), -sin(z), 0, 0,
		sin(z), cos(z), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	return Rz * Ry * Rx;
}

Matrix4d ZSTransform::CreateRotationMatrix(Vector3d& eulerAngles)
{
	return CreateRotationMatrix(eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
}

Matrix4d ZSTransform::CreateTransformationMatrix(Vector3d &rotation, Vector3d &scale, Vector3d &translation)
{
	return CreateTranslationMatrix(translation) * CreateScaleMatrix(scale) * CreateRotationMatrix(rotation);
}

Matrix4d ZSTransform::CreateInverseTransformationMatrix(Vector3d &rotation, Vector3d &scale, Vector3d &translation)
{
	Vector3d inv_scale;
	float x = 0, y = 0, z = 0;
	if (scale.x() == 0)
		x = 0;
	else
		x = 1.0 / scale.x();
	if (scale.y() == 0)
		y = 0;
	else
		y = 1.0 / scale.y();
	if (scale.z() == 0)
		z = 0;
	else
		z = 1.0 / scale.z();
	inv_scale << x, y, z;
	Vector3d inv_translation = -translation;
	// TransformationMatrix(M) = Translation(T) * Scale(S) * Rotation(R)
	// M = TSR
	// M_inv = (R_inv)(S_inv)(T_inv)
	return CreateInverseRotationMatrix(rotation) * CreateScaleMatrix(inv_scale) * CreateTranslationMatrix(inv_translation);
}
