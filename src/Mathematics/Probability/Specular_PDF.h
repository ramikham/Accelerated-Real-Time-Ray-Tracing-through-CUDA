//
// Created by Rami on 2/12/2024.
//

#ifndef CUDA_RAY_TRACER_SPECULAR_PDF_H
#define CUDA_RAY_TRACER_SPECULAR_PDF_H

#include "PDF.h"

// Supporting Function
// -----------------------------------------------------------------------
/// I found the following paper to very useful when implementing importance sampling for the Phong material:
/// Reference:  Importance Sampling of the Phong Reflectance Model: https://www.cs.princeton.edu/courses/archive/fall16/cos526/papers/importance.pdf
inline Vec3D weighted_direction(double specular_exponent) {
    // Generate two uniform random variables
    double u1 = random_double();
    double u2 = random_double();

    // Inverse transformation from CDF
    double phi = 2 * M_PI * u1;
    double theta = acos(pow(u2, 1/(specular_exponent+1)));

    double z = cos(theta);
    double y = sin(phi) * sin(theta);
    double x = cos(phi) * sin(theta);
  //  sin_called++; sin_called++; sin_called++;

    return {x, y, z};
}

class Specular_PDF : public PDF {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Specular_PDF(const Vec3D& w_o, const Vec3D& normal, double shininess) : w_o(unit_vector(w_o)), normal(unit_vector(normal)), shininess(shininess) {
        uvw = build_ONB(normal);
    }

    // Overridden Functions
    // -----------------------------------------------------------------------
    double PDF_value(const Vec3D& w_i) const override {
        // Calculates the angle between the incoming
        // direction (w_i) and the perfect reflection direction
        double specular_cos_alpha = dot_product(specular_reflection_direction(uvw[2], normal), unit_vector(w_i));

        // PDF for specular reflection
        return fmax(0.0, (shininess + 1) * std::pow(specular_cos_alpha, shininess) / (2 * M_PI));
    }

    Vec3D generate_a_random_direction_based_on_PDF() const override {
        return global_to_ONB_local(uvw, weighted_direction(shininess));
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    Vec3D w_o;                  // outgoing direction
    Vec3D normal;               // surface normal
    double shininess;           // how shiny the surface is
    std::vector<Vec3D> uvw;     // orthonormal basis
};


#endif //CUDA_RAY_TRACER_SPECULAR_PDF_H
