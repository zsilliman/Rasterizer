#include "ZSMath.h"

double ZSMath::clamp01(double t) {
    if (t > 1)
        return 1;
    else if (t < 0)
        return 0;
    return t;
}

double ZSMath::clamp(double t, double a, double b) {
    if (t > b)
        return b;
    else if (t < a)
        return a;
    return t;
}

int ZSMath::clampInt(int t, int a, int b) {
    if (t > b)
        return b;
    else if (t < a)
        return a;
    return t;
}

int ZSMath::NonNegativeModulo(int a, int n) {
    return ((a % n) + n) % n;
}

Eigen::Vector2d ZSMath::lerp(const Eigen::Vector2d& A, const Eigen::Vector2d& B, double t)
{
    t = clamp01(t);
    return A + (B - A) * t;
}

Eigen::Vector3d ZSMath::lerp(const Eigen::Vector3d& A, const Eigen::Vector3d& B, double t)
{
    t = clamp01(t);
    return A + (B - A) * t;
}

Eigen::Vector4d ZSMath::lerp(const Eigen::Vector4d& A, const Eigen::Vector4d& B, double t)
{
    t = clamp01(t);
    return A + (B - A) * t;
}

std::string ZSMath::toString(const Eigen::MatrixXd& mat) {
    std::stringstream ss;
    ss << mat;
    return ss.str();
}