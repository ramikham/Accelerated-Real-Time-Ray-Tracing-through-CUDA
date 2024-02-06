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
#include "Mathematics/Ray.h"
#include "Mathematics/Probability/Randomized_Algorithms.h"

// Include Constants
// -----------------------------------------------------------------------
const double infinity = std::numeric_limits<double>::infinity();
const double epsilon = std::numeric_limits<double>::epsilon();

// Support Functions
// -----------------------------------------------------------------------
inline double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

inline double clamp(double x, double min, double max) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

double uniform_pdf(){
    return 1/(2*M_PI);
}

// Gamma Correction
/// Reference: Fundamentals of Computer Graphics
// -----------------------------------------------------------------------
inline double gamma_2_correction(double linear_component) {
    return sqrt(linear_component);
}

/// Returns the direction of the reflected vector corresponding to the
/// incident ray and the normal vector at a point on the surface.
/// Reference: [5]
inline Vec3D specular_reflection_direction(const Vec3D& I, const Vec3D& N) {
    return I - 2*(dot_product(N,I)*N);
}

inline Vec3D diffuse_reflection_direction(const Vec3D& N) {
    return N + random_unit_vector();
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
    Vec3D unit_w = unit_vector(w);
    Vec3D a = (fabs(unit_w.x()) > 0.9) ? Vec3D(0,1,0) : Vec3D(1,0,0);
    Vec3D v = unit_vector(cross_product(unit_w, a));
    Vec3D u = cross_product(unit_w, v);
    std::vector<Vec3D> ONB = {u, v, unit_w};
    return ONB;
}

// Angle between
inline double angle_between(const Vec3D &u, const Vec3D &v) {
    return acos(dot_product(u, v)/(u.length() * v.length()));
}
#endif //CUDA_RAY_TRACER_UTILITIES_H
