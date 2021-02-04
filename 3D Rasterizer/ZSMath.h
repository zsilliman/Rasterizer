#pragma once
#include <Eigen/Dense>

namespace ZSMath {

	double clamp01(double t);

	double clamp(double t, double a, double b);

	int clampInt(int t, int a, int b);

	int NonNegativeModulo(int a, int n);

	Eigen::Vector2d lerp(const Eigen::Vector2d& A, const Eigen::Vector2d& B, double t);

	Eigen::Vector3d lerp(const Eigen::Vector3d& A, const Eigen::Vector3d& B, double t);

	Eigen::Vector4d lerp(const Eigen::Vector4d& A, const Eigen::Vector4d& B, double t);

	std::string toString(const Eigen::MatrixXd& mat);

	template <typename T> inline constexpr
	int Sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
};
