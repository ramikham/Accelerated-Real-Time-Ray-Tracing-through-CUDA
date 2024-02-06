//
// Created by Rami on 1/30/2024.
//

#ifndef CUDA_RAY_TRACER_PDF_H
#define CUDA_RAY_TRACER_PDF_H

#include "../../Utilities.h"

// An abstract class to represent the PDF for sampling directions based on a specific distribution.
// Essentially, the PDF is a function that describes the likelihood of a random variable taking on
// a particular value.

/// Reference: Ray Tracing: The Rest of Your Life
class PDF {
public:
    virtual ~PDF() {}
    virtual double PDF_value(const Vec3D& direction) const = 0;
    virtual Vec3D generate_a_random_direction_based_on_PDF() const = 0;
    virtual Vec3D generate_a_random_direction_based_on_PDF(const Vec3D& normal) const {
        return {0,0,0};
    };
};
#endif //CUDA_RAY_TRACER_PDF_H
