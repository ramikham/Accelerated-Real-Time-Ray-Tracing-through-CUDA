//
// Created by Rami on 1/30/2024.
//

#ifndef CUDA_RAY_TRACER_MIXTURE_PDF_H
#define CUDA_RAY_TRACER_MIXTURE_PDF_H

#include "PDF.h"

class Mixture_PDF : public PDF {
public:
    Mixture_PDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    double PDF_value(const Vec3D &direction) const override {
        return 0.5 * p[0]->PDF_value(direction) + 0.5*p[1]->PDF_value(direction);
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        return Vec3D();
    }

private:
    std::shared_ptr<PDF> p[2];
};

#endif //CUDA_RAY_TRACER_MIXTURE_PDF_H
