//
// Created by Rami on 10/7/2023.
//

#ifndef CUDA_RAY_TRACER_DIFFUSE_H
#define CUDA_RAY_TRACER_DIFFUSE_H

#include "Material.h"

class Diffuse : public Material {
public:
    Diffuse(const Color& surface_color) : surface_color(surface_color) {}

    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray) const override {
        Vec3D reflection_direction = diffuse_reflection_direction(intersection_info.normal);

        scattered_ray = Ray(intersection_info.p, reflection_direction);

       // shading_color = surface_color * std::fmax(0, dot_product(intersection_info.normal, reflection_direction)) / M_PI;
        shading_color = surface_color;
        return true;
    }

private:
    // Helper Functions

    // Data Members
    Color surface_color;          // = surface color = diffuse coefficient = diffuse color
};


#endif //CUDA_RAY_TRACER_DIFFUSE_H
