//
// Created by Rami on 10/10/2023.
//

#ifndef CUDA_RAY_TRACER_SPECULAR_H
#define CUDA_RAY_TRACER_SPECULAR_H

#include "Material.h"

class Specular : public Material {
public:
    Specular(const Color& surface_color, double glossy_reflection_fraction, double index_of_refraction) : surface_color(
            surface_color), glossy_reflection_fraction(glossy_reflection_fraction), index_of_refraction(
            index_of_refraction) {
        if (glossy_reflection_fraction > 1) {
            glossy_reflection_fraction = 1;
            reflection = true;
            refraction = false;
        }

        if (glossy_reflection_fraction < 1) {
            reflection = true;
            refraction = false;
        }
    }

    ~Specular() override = default;

    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf) const override {
        if (reflection) {
            // Calculate the direction of the reflected vector
            Vec3D R = specular_reflection_direction(unit_vector(incident_ray.get_ray_direction()),
                                                    intersection_info.normal);

            // Enable glossy reflection (mirror: flossy_reflection_fraction = 0)
            R += glossy_reflection_fraction * random_unit_vector();

            // Initialize the reflect ray
            scattered_ray = Ray(intersection_info.p, R);

            // A specular material (mirror) reflects the same color
            shading_color = surface_color;

            // Reflection inside the object surface do not illuminate the scene
            bool illumination = dot_product(scattered_ray.get_ray_direction(), intersection_info.normal) > 0;
            return illumination;
        }

        return false;
    }

public:
    // Data Members
    bool reflection;
    bool refraction;
    Color surface_color;
    double glossy_reflection_fraction;
    double index_of_refraction;
};

#endif //CUDA_RAY_TRACER_SPECULAR_H
