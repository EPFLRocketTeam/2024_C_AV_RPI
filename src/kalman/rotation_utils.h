#ifndef ROTATION_UTILS_H
#define ROTATION_UTILS_H

#include <iostream>
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
};

Eigen::Matrix3f quat_to_matrix(const Quaternion& q);
Quaternion rot_matrix_to_quat(const Eigen::Matrix3f& M);

// the azimuth is the (oriented) angle between the north vector and the projection of the rocket's up vector on the horizontal plane
// positive counter clockwise, i.e increases from the north to the west
float azimuth_of_quaternion(const Quaternion& q); 
// the pitch is the (non-oriented) angle between the rocket's up vector and the horizontal plane
float pitch_of_quaternion(const Quaternion& q);

#endif // ROTATION_UTILS_H
