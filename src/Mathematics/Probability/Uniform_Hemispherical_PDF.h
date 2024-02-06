//
// Created by Rami on 2/1/2024.
//

#ifndef CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H
#define CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H

#include "PDF.h"

class Uniform_Hemispherical_PDF : public PDF {
public:
    Uniform_Hemispherical_PDF(const Vec3D& intersection_normal) : intersection_normal(intersection_normal) {
        uvw = build_ONB(intersection_normal);
    }

    double PDF_value(const Vec3D &direction) const override {
        return 1/(2*M_PI);
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        return global_to_ONB_local(uvw, random_bla_bla());
    }

    Vec3D generate_a_random_direction_based_on_PDF(const Vec3D &normal) const override {
        return random_on_hemisphere(normal);
    }
private:
    Vec3D random_bla_bla() const {
        auto r1 = random_double();
        auto r2 = random_double();

        auto phi = 2 * pi * r1;
        auto theta = acos(1-r2);

        // Convert spherical coordinates to Cartesian coordinates
        auto x = sin(theta) * cos(phi);
        auto y = sin(theta) * sin(phi);
        auto z = cos(theta);


        return {x,y,z};
    }
    std::vector<Vec3D> uvw;
    Vec3D intersection_normal;
};


#endif //CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_PDF_H
