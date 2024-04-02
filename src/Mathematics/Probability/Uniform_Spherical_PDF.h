//
// Created by Rami on 1/30/2024.
//

#ifndef CUDA_RAY_TRACER_UNIFORM_SPHERICAL_PDF_H
#define CUDA_RAY_TRACER_UNIFORM_SPHERICAL_PDF_H

#include "PDF.h"

class Uniform_Spherical_PDF : public PDF {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Uniform_Spherical_PDF() {}

    // Overridden Functions
    // -----------------------------------------------------------------------
    double PDF_value(const Vec3D &direction) const override {
        return 1 / (4 * M_PI);
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        return random_unit_vector();
    }
};

#endif //CUDA_RAY_TRACER_UNIFORM_SPHERICAL_PDF_H
