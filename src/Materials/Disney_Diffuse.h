//
// Created by Rami on 3/29/2024.
//

#ifndef CUDA_RAY_TRACER_DISNEY_DIFFUSE_H
#define CUDA_RAY_TRACER_DISNEY_DIFFUSE_H

#include "Material.h"
#include "../Mathematics/Probability/Cosine_Weighted_PDF.h"

/// References:              - Physically Based Shading at Disney
///                          - Crash Course in BRDF Implementation
class Disney_Diffuse : public Material {
public:
    Disney_Diffuse(const Color& surface_color, double roughness) : surface_color(surface_color), roughness(roughness) {}

    bool evaluate(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                  Ray &scattered_ray, MATERIAL_TYPE &type, double &pdf,
                  std::shared_ptr<PDF> &surface_pdf_ptr) const override {
        // Generate a scattered ray with a direction from the corresponding PDF
        surface_pdf_ptr = std::make_shared<Cosine_Weighted_PDF>(intersection_info.normal);
        Vec3D scatter_direction = surface_pdf_ptr->generate_a_random_direction_based_on_PDF();
        scattered_ray = Ray(intersection_info.p, unit_vector(scatter_direction), incident_ray.get_time());

        // Get the PDF value for the generated scattered ray direction (the sampling PDF)
        pdf = surface_pdf_ptr->PDF_value(scattered_ray.get_ray_direction());

        // Set the shading color to the surface color multiplied by the diffuse reflectance
        shading_color = surface_color;

        return true;
    }

    double pdf(const Ray &incident_ray, const Intersection_Information &intersection_info, const Ray &scattered_ray) const override {
        // DEPRECATED: Only needed if you will use the OLD formulation of diffuse (commented above).
        // The new formulation used the PDF associated with the generated scattered ray direction.

        auto cos_theta = dot_product(intersection_info.normal, unit_vector(scattered_ray.get_ray_direction()));
        return cos_theta < 0 ? 0 : cos_theta/M_PI;
    }

    Vec3D BRDF(const Ray &incident_ray, const Intersection_Information &intersection_information, const Ray &scattered_ray, Color& attenuated_color) const override {
        // Evaluate Disney's diffuse reflectance
        double disney_diffuse = eval_Disney_diffuse(intersection_information.normal, unit_vector(scattered_ray.get_ray_direction()),
                                                    unit_vector(incident_ray.get_ray_direction()));

        auto cos_theta = dot_product(intersection_information.normal, unit_vector(scattered_ray.get_ray_direction()));
        // std::cout << disney_diffuse * attenuated_color * cos_theta/M_PI << std::endl;
        return cos_theta < 0 ? Vec3D(0,0,0) : disney_diffuse * attenuated_color * cos_theta/M_PI;
    }

private:
    // Evaluate Disney's diffuse reflectance based on the provided formula
    double eval_Disney_diffuse(const Vec3D& normal, const Vec3D& scatter_direction, const Vec3D& incident_direction) const {
        Vec3D viewer_direction = -incident_direction;
        Vec3D half_vector = unit_vector(viewer_direction + scatter_direction);

        // std::cout << "H . scatter = " << dot_product(half_vector, scatter_direction) << std::endl;
        // std::cout << "N . incident = " << dot_product(normal, incident_direction) << std::endl;
        // std::cout << "N . scatter = " << dot_product(normal, scatter_direction) << std::endl;

        // I FIXED the BUGG :) The problem was that the incident direction should have been reversed, so i used  Vec3D viewer_direction = -incident_direction;

        // Clamp N dot products to prevent numerical instability
        double FD90MinusOne = 2.0 * roughness * saturate(dot_product(half_vector, scatter_direction)) * saturate(dot_product(half_vector, scatter_direction)) - 0.5;
        double FDL = 1.0 + (FD90MinusOne * pow(1.0 - dot_product(normal, viewer_direction), 5.0));
        double FDV = 1.0 + (FD90MinusOne * pow(1.0 - dot_product(normal, scatter_direction), 5.0));
        return FDL * FDV;
    }

    Color surface_color;
    double roughness;                            // Roughness parameter for Disney's diffuse model
};

#endif //CUDA_RAY_TRACER_DISNEY_DIFFUSE_H
