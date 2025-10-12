#ifndef ROTATION_UTILS_H
#define ROTATION_UTILS_H

#include <Eigen/Dense>


class Quaternion {
public:
    float scalar;
    Eigen::Vector3f vector;

    Quaternion();
    Quaternion(float s, float x, float y, float z);
    Quaternion(float s, const Eigen::Vector3f& v);

    Quaternion operator+(const Quaternion& other) const;
    Quaternion operator*(float s) const;
    Quaternion operator*(const Quaternion& other) const;

    Quaternion normalised() const;
    // Quaternion conjugate() const;
};

Eigen::Matrix3f quat_to_matrix(const Quaternion& q);
Quaternion rot_matrix_to_quat(const Eigen::Matrix3f& M);

Eigen::Matrix3f skew_symmetric(const Eigen::Vector3f& v);

// the azimuth is the (oriented) angle between the north vector and the projection of the rocket's up vector on the horizontal plane
// positive clockwise, i.e increases from the north to the east
float azimuth_of_quaternion(const Quaternion& q); 
// the pitch is the (non-oriented) angle between the rocket's up vector and the horizontal plane
float pitch_of_quaternion(const Quaternion& q);
// rotation about the rocket's up axis, chosen here as the angle needed
// to bring the east vector into the horizontal plane.
float roll_of_quaternion(const Quaternion& q);
#endif // ROTATION_UTILS_H
