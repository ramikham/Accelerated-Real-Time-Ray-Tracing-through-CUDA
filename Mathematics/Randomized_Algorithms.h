//
// Created by Rami on 9/27/2023.
//

#ifndef CUDA_RAY_TRACER_RANDOMIZED_ALGORITHMS_H
#define CUDA_RAY_TRACER_RANDOMIZED_ALGORITHMS_H

#include "../Utilities.h"
#include <random>

/// A function that returns a random double between [lower_range,upper_range).
inline double random_double(double lower_bound, double upper_bound) {
    std::random_device dev;
    static std::mt19937 rand_eng(dev());
    static std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    double r = unif(rand_eng);

    return r;
}

/// A function that returns a random double between [0.0,1.0).
inline double random_double() {
    return random_double(0,1);
}

/// A function that returns a random vector with each of its components in the range: [lower_bound,upper_bound).
static Vec3D random_vector_in_range(double lower_bound, double upper_bound) {
    return {
            random_double(lower_bound, upper_bound),
            random_double(lower_bound, upper_bound),
            random_double(lower_bound, upper_bound)
            };
}

/// A function that returns a random vector with each if its components in the range: [lower_bound,upper_bound).
static Vec3D random_vector_in_range() {
    return {
            random_double(),
            random_double(),
            random_double()
            };
}

/// A function that returns a random vector on the unit sphere.
/// Reference: [4]
inline Vec3D random_unit_vector() {
    bool done = false;
    while (!done) {
        auto random_vector = random_vector_in_range(-1,1);
        if (random_vector.length_squared() < 1)
            return unit_vector(random_vector);
    }
}
#endif //CUDA_RAY_TRACER_RANDOMIZED_ALGORITHMS_H
