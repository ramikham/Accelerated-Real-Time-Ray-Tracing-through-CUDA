//
// Created by Rami on 1/30/2024.
//

#ifndef CUDA_RAY_TRACER_COSINE_WEIGHTED_PDF_H
#define CUDA_RAY_TRACER_COSINE_WEIGHTED_PDF_H

#include "PDF.h"

class Cosine_Weighted_PDF : public PDF {
public:
    Cosine_Weighted_PDF(const Vec3D& w) {
        uvw = build_ONB(w);
    }

    double PDF_value(const Vec3D& direction) const override {
        auto cosine_theta = dot_product(uvw[2], unit_vector(direction));
        return fmax(0, cosine_theta/M_PI);
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        return global_to_ONB_local(uvw, cosine_weighted_direction());
    }

private:
    std::vector<Vec3D> uvw;
};



#endif //CUDA_RAY_TRACER_COSINE_WEIGHTED_PDF_H
