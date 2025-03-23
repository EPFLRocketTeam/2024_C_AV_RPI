#include "rotation_utils.h"
#include <Eigen/Dense>

// Quaternion class implementation
Quaternion::Quaternion() : scalar(1.0), vector(Eigen::Vector3f::Zero()) {}

Quaternion::Quaternion(float s, float x, float y, float z) : scalar(s), vector(x, y, z) {}

Quaternion::Quaternion(float s, const Eigen::Vector3f& v) : scalar(s), vector(v) {}

Quaternion Quaternion::operator+(const Quaternion& other) const {
    return Quaternion(scalar + other.scalar, vector + other.vector);
}

Quaternion Quaternion::operator*(float s) const {
    return Quaternion(scalar * s, vector * s);
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    float s = scalar * other.scalar - vector.dot(other.vector);
    
    Eigen::Vector3f v = scalar * other.vector + other.scalar * vector + vector.cross(other.vector);
    
    return Quaternion(s, v);
}

Quaternion Quaternion::normalised() const {
    float norm = std::sqrt(scalar * scalar + vector.squaredNorm());
    if (norm < 1e-10) {
        return Quaternion(1.0, 0.0, 0.0, 0.0);
    }
    float invNorm = 1.0 / norm;
    return Quaternion(scalar * invNorm, vector * invNorm);
}

// Quaternion Quaternion::conjugate(const Quaternion& q) const {
//     return Quaternion(q.scalar, -q.vector[0], -q.vector[1], -q.vector[2]);
// }

Eigen::Matrix3f quat_to_matrix(const Quaternion& q) {
    float qi = q.vector(0), qj = q.vector(1), qk = q.vector(2), qw = q.scalar;
    
    Eigen::Matrix3f result;
    result << 1 - 2 * (qj * qj + qk * qk), 2 * (qi * qj - qk * qw), 2 * (qi * qk + qj * qw),
              2 * (qi * qj + qk * qw), 1 - 2 * (qi * qi + qk * qk), 2 * (qj * qk - qi * qw),
              2 * (qi * qk - qj * qw), 2 * (qj * qk + qi * qw), 1 - 2 * (qi * qi + qj * qj);
    
    return result;
}

Quaternion rot_matrix_to_quat(const Eigen::Matrix3f& M) {
    float T = M.trace(); // Trace of M
    // Determine the max between the trace and the diagonal coefficients to ensure numerical stability
    float temp1 = std::max(M(0,0), M(1,1));
    float temp2 = std::max(M(2,2), T);
    float m = std::max(temp1, temp2);
    float qmax = 0.5 * sqrt(1.0 - T + 2.0 * m);

    if (m == M(0,0))
    {
        return Quaternion( 
            (M(0,1) + M(1,0)) / (4.0 * qmax), 
            (M(0,2) + M(2,0)) / (4.0 * qmax), 
            (M(2,1) - M(1,2)) / (4.0 * qmax),
            qmax);
    }
    else if (m == M(1,1))
    {
        return Quaternion( 
            (M(0,2) - M(2,0)) / (4.0 * qmax),
            (M(0,1) + M(1,0)) / (4.0 * qmax),
            qmax,
            (M(1,2) + M(2,1)) / (4.0 * qmax));

    }
    else if (m == M(2,2))
    {
        return Quaternion( 
            (M(1,0) - M(0,1)) / (4.0 * qmax),
            (M(0,2) + M(2,0)) / (4.0 * qmax),
            (M(1,2) + M(2,1)) / (4.0 * qmax),
            qmax);
    }
    else
    {
        return Quaternion( 
            qmax,
            (M(2,1) - M(1,2)) / (4.0 * qmax),
            (M(0,2) - M(2,0)) / (4.0 * qmax),
            (M(1,0) - M(0,1)) / (4.0 * qmax));
    }
}

Eigen::Matrix3f skew_symmetric(const Eigen::Vector3f& v) {
    Eigen::Matrix3f m;
    m <<  0,    -v(2),  v(1),
          v(2),  0,    -v(0),
         -v(1),  v(0),  0;
    return m;
}



// E : east vector, N : north vector, U : up vector
// X and Y : transversal plane of the rocket frame
// Z : longitudinal vector of the rocket frame (tail to nose)
// vectors are expressed in the earth frame so U = (0, 0, 1).T and Z = R^T U for example


// We define the Euler angle sequence following the Z-Y-Z convention:
// azimuth (or yaw) then pitch then roll
// so R = Rz(azimuth) * Ry(pi/2 - pitch) * Rz(roll)

float azimuth_of_quaternion(const Quaternion& q) {
    // = atan2((Z x N) • U, N • Z) = atan2((R^T U x N) • U, N • R^T U) = atan2(R(0,2),R(1,2))
    return std::atan2(2.0f * (q.vector[0] * q.vector[2] + q.vector[1] * q.scalar),
                      2.0f * (q.vector[1] * q.vector[2] - q.vector[0] * q.scalar));
}

float pitch_of_quaternion(const Quaternion& q) {
    // = asin(Z • U) = asin(R^T U • U) = asin(R(2,2))
    return std::asin(1.0f - 2.0f * (q.vector[0] * q.vector[0] + q.vector[1] * q.vector[1]));
}


// note : this is wrong, TODO : correct it
float roll_of_quaternion(const Quaternion& q) {
    // last rotation around Z after applying yaw and pitch
    return std::atan2(  2.0f * (q.vector[0] * q.vector[2] - q.vector[1] * q.scalar),
                      - 2.0f * (q.vector[1] * q.vector[2] + q.vector[0] * q.scalar));
}