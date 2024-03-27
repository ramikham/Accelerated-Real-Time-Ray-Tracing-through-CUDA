//
// Created by Rami on 3/27/2024.
//

#ifndef CUDA_RAY_TRACER_DIFFUSE_WITH_TEXTURE_H
#define CUDA_RAY_TRACER_DIFFUSE_WITH_TEXTURE_H

#include "Material.h"
#include "../Mathematics/Probability/Cosine_Weighted_PDF.h"
#include "../Textures/Texture.h"

class Diffuse_With_Texture : public Material {
public:
    Diffuse_With_Texture(const std::shared_ptr<Texture>& surface_color) : surface_color(surface_color) {}

    bool evaluate(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                  Ray &scattered_ray, MATERIAL_TYPE &type, double &pdf, std::shared_ptr<PDF> &surface_pdf_ptr) const override {
        // Generate a scattered ray with a direction from the corresponding PDF
        surface_pdf_ptr = std::make_shared<Cosine_Weighted_PDF>(intersection_info.normal);          // A cosine-weighted distribution is physically correct
        Vec3D scatter_direction = surface_pdf_ptr->generate_a_random_direction_based_on_PDF();
        scattered_ray = Ray(intersection_info.p, unit_vector(scatter_direction), incident_ray.get_time());

        // Get the PDF value for the generated scattered ray direction (the sampling PDF)
        pdf = surface_pdf_ptr->PDF_value(scattered_ray.get_ray_direction());

        // Set the shading color to the surface texture color
        shading_color = surface_color->value_at(intersection_info.u, intersection_info.v, intersection_info.p);

        return true;
    }

    Vec3D BRDF(const Ray &incident_ray, const Intersection_Information &intersection_information, const Ray &scattered_ray, Color& attenuated_color) const override {
        auto cos_theta = dot_product(intersection_information.normal, unit_vector(scattered_ray.get_ray_direction()));
        return cos_theta < 0 ? Vec3D(0,0,0) : attenuated_color * cos_theta/M_PI;
    }

private:
    std::shared_ptr<Texture> surface_color;
};


#endif //CUDA_RAY_TRACER_DIFFUSE_WITH_TEXTURE_H
