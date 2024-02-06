//
// Created by Rami on 1/30/2024.
//

#ifndef CUDA_RAY_TRACER_PRIMITIVE_PDF_H
#define CUDA_RAY_TRACER_PRIMITIVE_PDF_H

#include "../../Primitives/Primitives_Group.h"
#include "PDF.h"

class Primitive_PDF : public PDF {
public:
    Primitive_PDF(const Primitive& primitives, const point3D& origin) : primitives(primitives), origin(origin) {
        if (std::isnan(origin.x()) || std::isnan(origin.y()) || std::isnan(origin.z())) {
            std::cerr << "Primitive_PDF: NaN detected in origin during construction." << std::endl;
            // POTENTIAL BUGGG!
        }
    }

    double PDF_value(const Vec3D& direction) const override {
        return primitives.PDF_value(origin, direction);
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        //  std::cout << "Group PDF: " << primitives.random(origin) << std::endl;
        //  std::cout << "Origin in primitive_pdf " << origin << std::endl;
        return primitives.random(origin);
    }

private:
    const Primitive& primitives;
    point3D origin;
};

#endif //CUDA_RAY_TRACER_PRIMITIVE_PDF_H
