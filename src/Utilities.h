//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_UTILITIES_H
#define CUDA_RAY_TRACER_UTILITIES_H

#include <iostream>
#include <fstream>
#include <limits>
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <omp.h>
#include <sstream>

// Include Headers
// -----------------------------------------------------------------------
#include "Mathematics/Vec3D.h"
#include "Mathematics/Vec2D.h"
#include "Mathematics/Ray.h"
#include "Mathematics/Probability/Randomized_Algorithms.h"

// Include Constants
// -----------------------------------------------------------------------
const double infinity = std::numeric_limits<double>::infinity();
const double epsilon = std::numeric_limits<double>::epsilon();

// Support Functions
// -----------------------------------------------------------------------
inline double degrees_to_radians(double degrees) {
    // Converts degrees to radians.

    return degrees * M_PI / 180.0;
}

inline double clamp(double x, double min, double max) {
    // Clamps a double to [min,max].

    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

double uniform_pdf(){
    // A uniform pdf over the hemisphere of directions.

    return 1/(2*M_PI);
}

point3D rotate_point_around_X(const point3D& p, double theta) {
    // A function that rotates a 3D point 'p' by an angle 'theta' around the X-axis.
    // Assumes that 'theta' is given in radians.

    double y = cos(theta) * p.y() - sin(theta) * p.z();
    double z = sin(theta) * p.y() + cos(theta) * p.z();

    return {p.x(), y, z};
}

point3D rotate_point_around_Y(const point3D& p, double theta) {
    // A function that rotates a 3D point 'p' by an angle 'theta' around the Y-axis.
    // Assumes that 'theta' is given in radians.

    double x = cos(theta) * p.x() + sin(theta) * p.z();
    double z = -sin(theta) * p.x() + cos(theta) * p.z();

    return {x, p.y(), z};
}

point3D rotate_point_around_Z(const point3D& p, double theta) {
    // A function that rotates a 3D point 'p' by an angle 'theta' around the Z-axis.
    // Assumes that 'theta' is given in radians.

    double x = cos(theta) * p.x() - sin(theta) * p.y();
    double y = sin(theta) * p.x() + cos(theta) * p.y();

    return {x, y, p.z()};
}

// Gamma Correction
/// Reference: Fundamentals of Computer Graphics
// -----------------------------------------------------------------------
inline double gamma_2_correction(double linear_component) {
    return sqrt(linear_component);
}

/// Reference: An Introduction to Ray Tracing
inline Vec3D specular_reflection_direction(const Vec3D& I, const Vec3D& N) {
    // Returns the direction of the reflected vector corresponding to the incident
    // ray and the normal vector at a point on the surface.

    return I - 2 * (dot_product(N,I)*N);
}

/// Reference (see Note below): https://github.com/vchizhov/Derivations/blob/master/Probability%20density%20functions%20of%20the%20projected%20offset%20disk%2C%20circle%2C%20ball%2C%20and%20sphere.pdf
inline Vec3D diffuse_reflection_direction(const Vec3D& N) {
    // Calculates a diffuse reflection direction by perturbing the normal N by some
    // random unit vector.
    // Note: This is not a cosine weighted distribution of generating
    // random directions. It can be shown that it is proportional to cos^3(θ)sinθ.

    return N + random_unit_vector();
}

inline double angle_between(const Vec3D &u, const Vec3D &v) {
    // Calculates the angle between the two vectors u and b,
    // θ = cos^1[(a.b)/(||a| ||b||)].

    return acos(dot_product(u, v)/(u.length() * v.length()));
}

// Orthonormal basis Construction Functions
/// References:          - Fundamentals of Computer Graphics - Section 2.4.5: Orthonormal Bases and Coordinate Frames
///                      - Fundamentals of Computer Graphics - Section 2.4.6: Constructing a Basis from a Single Vector
// -----------------------------------------------------------------------
inline Vec3D global_to_ONB_local(Vec3D ONB_axes[3], double a, double b, double c){
    return a*ONB_axes[0] + b*ONB_axes[1] + c*ONB_axes[2];
}

inline Vec3D global_to_ONB_local(const std::vector<Vec3D>& ONB_axes, const Vec3D& v){
    //std::cout << "INSIDE MY FUNCTION: " << ONB_axes[0] << " , " << ONB_axes[1] << " , " << ONB_axes[2] << std::endl;
    return v.x()*ONB_axes[0] + v.y()*ONB_axes[1] + v.z()*ONB_axes[2];
}

std::vector<Vec3D> build_ONB(const Vec3D& w){
    // Constructs and returns an orthonormal basis from the given vector w.

    Vec3D unit_w = unit_vector(w);
    Vec3D a = (fabs(unit_w.x()) > 0.9) ? Vec3D(0,1,0) : Vec3D(1,0,0);
    Vec3D v = unit_vector(cross_product(unit_w, a));
    Vec3D u = cross_product(unit_w, v);
    std::vector<Vec3D> ONB = {u, v, unit_w};
    return ONB;
}


#endif //CUDA_RAY_TRACER_UTILITIES_H
