//
// Created by Rami on 9/27/2023.
//

#ifndef CUDA_RAY_TRACER_RANDOMIZED_ALGORITHMS_H
#define CUDA_RAY_TRACER_RANDOMIZED_ALGORITHMS_H

#include "../../Utilities.h"
#include <random>
static int num_calls_rand_double = 0;
// Constants
// -----------------------------------------------------------------------
const double TWO_PI = 2 * M_PI;

// Randomized Algorithms
// -----------------------------------------------------------------------

inline double random_double() {
    // Returns a random double between [0.0,1.0).
    // num_calls_rand_double++;
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double lower_bound, double upper_bound) {
    // Returns a random double between [lower_range,upper_range).

    return lower_bound + (upper_bound-lower_bound)*random_double();
}

static Vec3D random_vector_in_range(double lower_bound, double upper_bound) {
    // Returns a random vector with each of its components in the range: [lower_bound,upper_bound).

    return {
            random_double(lower_bound, upper_bound),
            random_double(lower_bound, upper_bound),
            random_double(lower_bound, upper_bound)
            };
}

inline int random_int_in_range(double min, double max) {
    // Returns a random int in [min, max]

    return static_cast<int>(random_double(min, max+1));
}

static Vec3D random_vector_in_range() {
    // Returns a random vector with each if its components in the range: [lower_bound,upper_bound).

    return {
            random_double(),
            random_double(),
            random_double()
            };
}

inline Vec3D random_unit_vector() {
    // Returns a random vector on the unit sphere of directions.

    do {
        auto random_vector = random_vector_in_range(-1, 1);
        if (random_vector.length_squared() < 1)
            return unit_vector(random_vector);
    } while (true);
}

inline Vec3D random_on_hemisphere(const Vec3D& normal) {
    // Returns a random vector on the unit hemisphere of directions.

    Vec3D on_unit_sphere = random_unit_vector();

    // In the same hemisphere as the normal?
    if (dot_product(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// Peter Shirley's method to generate cosine-weighted directions
inline Vec3D random_cosine_direction() {
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2*M_PI*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);
    auto z = sqrt(1-r2);
   // std::cout << x << " " << y << " " << z << std::endl;
    return Vec3D(x, y, z);
}

static int sin_called = 0;
// My method
inline Vec3D cosine_weighted_direction() {
    double r1 = random_double();
    double r2 = random_double();

    double phi = TWO_PI * r1;
    double sqrt_r2 = sqrt(r2);
    double cos_phi = cos(phi);
   // sin_called++;

    return {cos_phi * sqrt_r2, sin(phi) * sqrt_r2, sqrt(1.0 - r2)};
}

inline Vec3D direction_on_hemisphere() {
    // Generates a uniformly distributed direction on the unit hemisphere

    auto r1 = random_double();
    auto phi = 2 * M_PI * r1;

    auto r2 = random_double();

    // Convert spherical coordinates to Cartesian coordinates
    auto x = sqrt(1-(r2*r2)) * cos(phi);
    auto y = sqrt(1-(r2*r2)) * sin(phi);
    auto z = 1 - r2;


    return {x,y,z};
}

#endif //CUDA_RAY_TRACER_RANDOMIZED_ALGORITHMS_H
