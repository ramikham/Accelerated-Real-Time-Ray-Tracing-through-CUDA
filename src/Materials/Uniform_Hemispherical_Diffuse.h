//
// Created by Rami on 1/28/2024.
//

#ifndef CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_DIFFUSE_H
#define CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_DIFFUSE_H

#include "Material.h"

class Uniform_Hemispherical_Diffuse : public Material {
public:
    Uniform_Hemispherical_Diffuse(const Color& surface_color) : surface_color(surface_color) {}

    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf) const override {
        material_type = UNIFORM_HEMPISPHERICAL_DIFFUSE;
        Vec3D reflection_direction = random_on_hemisphere(intersection_info.normal);
        scattered_ray = Ray(intersection_info.p, reflection_direction);
        pdf = uniform_pdf();
        shading_color = surface_color;
        return true;
    }

    Vec3D BRDF(const Ray &incident_ray, const Intersection_Information &intersection_information,
               const Ray &scattered_ray, Color& attenuated_color) const override {
        return attenuated_color * (1 / (2*M_PI));
    }

private:
    // Data Members
    Color surface_color;          // = surface color = diffuse coefficient = diffuse color
};


#endif //CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_DIFFUSE_H
