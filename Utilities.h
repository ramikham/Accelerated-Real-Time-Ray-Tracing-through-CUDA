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

// Include Headers
// -----------------------------------------------------------------------
#include "Mathematics/Vec3D.h"
#include "Mathematics/Ray.h"
#include "Mathematics/Randomized_Algorithms.h"

// Include Constants
// -----------------------------------------------------------------------
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Support Functions
// -----------------------------------------------------------------------
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double clamp(double x, double min, double max) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
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
enum Material_Type {DIFFUSE, SPECULAR};
#endif //CUDA_RAY_TRACER_UTILITIES_H
