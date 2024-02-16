//
// Created by Rami on 2/9/2024.
//

#ifndef CUDA_RAY_TRACER_VEC2D_H
#define CUDA_RAY_TRACER_VEC2D_H

#include <iostream>
#include <cmath>

class Vec2D {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Vec2D() : V{0, 0} {}         // Default constructor

    Vec2D(double v_x, double v_y) : V{v_x, v_y} {}

    // Getters
    // -----------------------------------------------------------------------
    double x() const { return V[0]; }

    double y() const { return V[1]; }

    // Overloaded operators
    // -----------------------------------------------------------------------
    Vec2D operator-() const { return Vec2D(-V[0], -V[1]); }

    Vec2D& operator+=(const Vec2D& v) {
        V[0] += v[0];
        V[1] += v[1];

        return *this;
    }

    Vec2D& operator*=(const double c) {
        V[0] *= c;
        V[1] *= c;

        return *this;
    }

    Vec2D operator/=(const double c) {
        return *this *= 1/c;        // reciprocal multiplication
    }

    double operator[](int i) const { return V[i]; }

    double& operator[](int i) { return V[i]; }

    // Vector operations
    // -----------------------------------------------------------------------
    double length_squared() const { return V[0] * V[0] + V[1] * V[1]; }

    double length() const { return std::sqrt(length_squared()); }
public:
    // Data Members
    // -----------------------------------------------------------------------
    double V[2];
};

// Cross product and dot product (not applicable for 2D vectors)
// ------------------------------------------------------------------------
// Other vector operations
// ------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Vec2D& v) {
    return std::cout << v[0] << " " << v[1] << std::endl;
}

inline Vec2D operator+(const Vec2D& u, const Vec2D& v) {
    return {u[0] + v[0], u[1] + v[1]};
}

inline Vec2D operator-(const Vec2D& u, const Vec2D& v) {
    return {u[0] - v[0], u[1] - v[1]};
}

inline Vec2D operator*(const Vec2D& u, const Vec2D& v) {
    return {u[0] * v[0], u[1] * v[1]};
}

inline Vec2D operator*(double t, const Vec2D& v) {
    return {t * v[0], t * v[1]};
}

inline Vec2D operator*(const Vec2D& v, double t) {
    return t * v;
}

inline Vec2D operator/(Vec2D v, double t) {
    return (1 / t) * v;
}

inline Vec2D unit_vector(Vec2D v) { return v / v.length(); }

inline Vec2D min(const Vec2D& a, const Vec2D& b) {
    return { fmin(a.x(), b.x()), fmin(a.y(), b.y()) };
}

inline Vec2D max(const Vec2D& a, const Vec2D& b) {
    return { fmax(a.x(), b.x()), fmax(a.y(), b.y()) };
}
#endif //CUDA_RAY_TRACER_VEC2D_H
