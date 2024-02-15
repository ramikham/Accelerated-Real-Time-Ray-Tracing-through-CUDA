//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_VEC3D_H
#define CUDA_RAY_TRACER_VEC3D_H

#include "../Utilities.h"

class Vec3D {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Vec3D() : V{0, 0, 0} {}     // Default constructor
    Vec3D(double v_x, double v_y, double v_z) : V{v_x, v_y, v_z} {}

    // Getters
    // -----------------------------------------------------------------------
    double x() const { return V[0]; }
    double y() const { return V[1]; }
    double z() const { return V[2]; }

    // Overloaded operators
    // -----------------------------------------------------------------------
    Vec3D operator-() const { return Vec3D(-V[0], -V[1], -V[2]); }
    Vec3D& operator+=(const Vec3D& v) {
        V[0] += v[0];
        V[1] += v[1];
        V[2] += v[2];

        return* this;
    }
    Vec3D& operator*=(const double c) {
        V[0] *= c;
        V[1] *= c;
        V[2] *= c;

        return* this;
    }
    Vec3D operator/=(const double c) {
        return* this *= 1/c;        // reciprocal multiplication
    }
    double operator[](int i) const { return V[i]; }
    double& operator[](int i) { return V[i]; }

    // Vector operations
    // -----------------------------------------------------------------------
    double length_squared() const { return V[0] * V[0] + V[1] * V[1] + V[2] * V[2]; }
    double length() const { return std::sqrt(length_squared()); }
public:
    // Data Members
    // -----------------------------------------------------------------------
    double V[3];
};

// Design Choice: make a point a vector (aliases)
// ------------------------------------------------------------------------
using point3D = Vec3D;
using Color = Vec3D;

// Cross product and dot product
// ------------------------------------------------------------------------
inline double dot_product(const Vec3D& u, const Vec3D& v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline Vec3D cross_product(const Vec3D& u, const Vec3D& v) {
    return Vec3D(
            u[1] * v[2] - u[2] * v[1],
            u[2] * v[0] - u[0] * v[2],
            u[0] * v[1] - u[1] * v[0]
    );
}

// Other vector operations
// ------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Vec3D& v) {
    return std::cout << v[0] << " " << v[1] << " " << v[2] << std::endl;
}

inline Vec3D operator+(const Vec3D& u, const Vec3D& v) {
    return {u[0] + v[0], u[1] + v[1], u[2] + v[2]};
}

inline Vec3D operator-(const Vec3D& u, const Vec3D& v) {
    return {u[0] - v[0], u[1] - v[1], u[2] - v[2]};
}

inline Vec3D operator*(const Vec3D u, const Vec3D& v) {
    return {u[0] * v[0], u[1] * v[1], u[2] * v[2]};
}

inline Vec3D operator*(double t, const Vec3D& v) {
    return {t * v[0], t * v[1], t * v[2]};
}

inline Vec3D operator*(const Vec3D& v, double t) {
    return t * v;
}

inline Vec3D operator/(Vec3D v, double t) {
    return (1 / t) * v;
}

inline Vec3D unit_vector(Vec3D v) { return v / v.length(); }

inline Vec3D min(const Vec3D& a, const Vec3D& b) {
    return { fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()) };
}

inline Vec3D max(const Vec3D& a, const Vec3D& b) {
    return { fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()) };
}


#endif //CUDA_RAY_TRACER_VEC3D_H
