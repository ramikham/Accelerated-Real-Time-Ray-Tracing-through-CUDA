//
// Created by Rami on 1/28/2024.
//

#ifndef CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_DIFFUSE_H
#define CUDA_RAY_TRACER_UNIFORM_HEMISPHERICAL_DIFFUSE_H

#include "Material.h"
#include "../Mathematics/Probability/Uniform_Spherical_PDF.h"
#include "../Mathematics/Probability/Uniform_Hemispherical_PDF.h"

class Uniform_Hemispherical_Diffuse : public Material {
public:
    Uniform_Hemispherical_Diffuse(const Color& surface_color) : surface_color(surface_color) {}

    /// Reference: 14.7.1: BRDF
    bool evaluate(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                  Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const override {

        // Generate a scattered ray with a direction from the corresponding PDF
        surface_pdf_ptr = std::make_shared<Uniform_Hemispherical_PDF>(intersection_info.normal);
        Vec3D scatter_direction = surface_pdf_ptr->generate_a_random_direction_based_on_PDF(intersection_info.normal);
        scattered_ray = Ray(intersection_info.p, scatter_direction);

        // Get the PDF value for the generated scattered ray direction
        pdf = uniform_pdf();            // A uniform hemispherical diffuse always has a pdf of 1/2π

        // Set the shading color to the surface color
        shading_color = surface_color;

        return true;
/*
        // OLD - BUT WORKING WITH ONLY radiance_background() or radiance()
        Vec3D reflection_direction = random_on_hemisphere(intersection_info.normal);
        scattered_ray = Ray(intersection_info.p, reflection_direction);
        pdf = uniform_pdf();
        shading_color = surface_color;
        return true;
*/
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
