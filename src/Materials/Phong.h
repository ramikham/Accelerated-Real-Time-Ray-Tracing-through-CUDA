//
// Created by Rami on 1/27/2024.
//

#ifndef CUDA_RAY_TRACER_PHONG_H
#define CUDA_RAY_TRACER_PHONG_H

#include "Material.h"

/// Reference: xxx
// -------------------------------------------------------------------------------
class Phong : public Material {
public:
    // Constructor
    // -------------------------------------------------------------------------------
    Phong(const Color &surfaceColor, float specularIntensity, float shininess) : surface_color(surfaceColor),
                                                                                 specular_intensity(specularIntensity),
                                                                                 shininess(shininess) {}
    // Overridden Functions
    // -------------------------------------------------------------------------------
    /// References:         - Fundamentals of Computer Graphics - Section 5.2.2: Specular Reflection
    ///                     - Fundamentals of Computer Graphics - Section 5.2.3: Calculating Shading
    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const override {
        surface_pdf_ptr = nullptr;              // no BRDF
        material_type = PHONG;

        // Diffuse reflection
        Vec3D reflection_direction = diffuse_reflection_direction(intersection_info.normal);
        scattered_ray = Ray(intersection_info.p, reflection_direction);
        float diffuse_component = std::fmax(0, dot_product(intersection_info.normal, reflection_direction));

        // Specular reflection
        Vec3D viewer_direction = -incident_ray.get_ray_direction();
        Vec3D halfway_direction = unit_vector(viewer_direction + scattered_ray.get_ray_direction());
        float specular_component = std::pow(std::fmax(0, dot_product(intersection_info.normal, halfway_direction)), shininess);

        // Combine diffuse and specular reflections
        shading_color = surface_color * (diffuse_component + specular_component * specular_intensity) / M_PI;

        return true;
    }

    double pdf(const Ray &incident_ray, const Intersection_Information &intersection_info,
               const Ray &scattered_ray) const override {
        // TODO: Implement
    }

    Vec3D
    BRDF(const Ray &incident_ray, const Intersection_Information &intersection_information, const Ray &scattered_ray,
         Color &attenuated_color) const override {
        // TODO: Implement
    }

private:
    // Data Members
    // -------------------------------------------------------------------------------
    Color surface_color;
    float specular_intensity;
    float shininess;
};

#endif //CUDA_RAY_TRACER_PHONG_H
