//
// Created by Rami on 2/1/2024.
//

#ifndef CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H
#define CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H

#include "PDF.h"

class Uniform_Hemispherical_PDF : public PDF {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Uniform_Hemispherical_PDF(const Vec3D& intersection_normal) : intersection_normal(intersection_normal) {
        uvw = build_ONB(intersection_normal);
    }

    // Overridden Functions
    // -----------------------------------------------------------------------
    double PDF_value(const Vec3D &direction) const override {
        return 1/(2*M_PI);
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        return global_to_ONB_local(uvw, direction_on_hemisphere());
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    std::vector<Vec3D> uvw;
    Vec3D intersection_normal;
};


#endif //CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H
