//
// Created by Rami on 1/27/2024.
//

#ifndef CUDA_RAY_TRACER_PHONG_H
#define CUDA_RAY_TRACER_PHONG_H

#include "Material.h"
#include "../Mathematics/Probability/Specular_PDF.h"

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
    ///                     - Importance Sampling of the Phong Reflectance Model: https://www.cs.princeton.edu/courses/archive/fall16/cos526/papers/importance.pdf
    bool evaluate(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                  Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const override {


            double u = random_double();                 // generate a random variable u ∈ [0,1]

            double k_d = 0.5;           // diffuse component
            double k_s = 0.5;           // specular component

            if (u < k_d) {
                // Take a diffuse sample and compute its contribution
                surface_pdf_ptr = std::make_shared<Cosine_Weighted_PDF>(intersection_info.normal);
                Vec3D scatter_direction = surface_pdf_ptr->generate_a_random_direction_based_on_PDF();
                scattered_ray = Ray(intersection_info.p, unit_vector(scatter_direction), incident_ray.get_time());
                pdf = surface_pdf_ptr->PDF_value(scattered_ray.get_ray_direction());
                shading_color = surface_color;
            } else if (k_d <= u && u < k_d + k_s) {
                // Take a specular sample and compute its contribution
                surface_pdf_ptr = std::make_shared<Specular_PDF>(incident_ray.get_ray_direction(), intersection_info.normal, shininess);
                Vec3D scatter_direction = surface_pdf_ptr->generate_a_random_direction_based_on_PDF();
                scattered_ray = Ray(intersection_info.p, unit_vector(scatter_direction), incident_ray.get_time());
                pdf = surface_pdf_ptr->PDF_value(scattered_ray.get_ray_direction());
                shading_color = surface_color;
            } else {
                // Contribution is 0
                return false;
            }

            return true;

            /* OLD - but works. Use it if you don't wish to do importance sampling for Phong materials. */
            /*
            surface_pdf_ptr = nullptr;              // no BRDF
            material_type = PHONG;                  // Material Type

            // Diffuse reflection
            Vec3D reflection_direction = diffuse_reflection_direction(intersection_info.normal);
            scattered_ray = Ray(intersection_info.p, reflection_direction);
            float diffuse_component = std::fmax(0, dot_product(intersection_info.normal, reflection_direction));

            // Specular reflection
            Vec3D viewer_direction = -incident_ray.get_ray_direction();
            Vec3D halfway_direction = unit_vector(viewer_direction + scattered_ray.get_ray_direction());        // check
            float specular_component = std::pow(std::fmax(0, dot_product(intersection_info.normal, halfway_direction)), shininess);

            // Combine diffuse and specular reflections
            shading_color = surface_color * (diffuse_component + specular_component * specular_intensity) / M_PI;

            return true;
            */
        }

    /// Reference:  - Crash Course in BRDF Implementation: https://boksajak.github.io/files/CrashCourseBRDF.pdf
    Vec3D BRDF(const Ray &incident_ray, const Intersection_Information &intersection_information, const Ray &scattered_ray,
         Color &attenuated_color) const override {
        // Computes the BRDF of a Phong material, which is given be the equation:
        //                BrdfPhong = specular_reflectance * pow(dot(R, V), shininess) * dot(N, L)

        // Compute cos(α), where α = shininess
        Vec3D viewer_direction = incident_ray.get_ray_direction();
        Vec3D reflection_direction = specular_reflection_direction(unit_vector(viewer_direction), intersection_information.normal);

        double cos_alpha = dot_product(unit_vector(scattered_ray.get_ray_direction()), reflection_direction);

        // Ensure cos_alpha is within valid range to avoid numerical issues
        cos_alpha = clamp(cos_alpha, 0.0, 1.0);

        //   std::cout << attenuated_color * (specular_intensity * std::pow(cos_alpha, shininess)) << std::endl;

        // Phong BRDF
        return attenuated_color * (specular_intensity * std::pow(cos_alpha, shininess));

    }

private:
    // Data Members
    // -------------------------------------------------------------------------------
    Color surface_color;
    float specular_intensity;
    float shininess;
};

#endif //CUDA_RAY_TRACER_PHONG_H
