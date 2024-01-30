//
// Created by Rami on 1/30/2024.
//

#ifndef CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H
#define CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H

#include "PDF.h"

class Uniform_Hemispherical_PDF : PDF {
public:
    Uniform_Hemispherical_PDF() {}

private:
    double PDF_value(const Vec3D &direction) const override {
        return 1/(2*M_PI);
    }

    Vec3D generate_a_random_direction_based_on_PDF(const Vec3D &normal) const override {
        return random_on_hemisphere(normal);
    }

};

#endif //CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H
