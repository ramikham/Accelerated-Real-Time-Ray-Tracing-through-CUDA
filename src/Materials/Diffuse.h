//
// Created by Rami on 10/7/2023.
//

#ifndef CUDA_RAY_TRACER_DIFFUSE_H
#define CUDA_RAY_TRACER_DIFFUSE_H

#include "Material.h"
#include "../onb.h"
#include "../Mathematics/Probability/Cosine_Weighted_PDF.h"

class Diffuse : public Material {
public:
    Diffuse(const Color& surface_color) : surface_color(surface_color) {}

    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const override {
        /*
        onb uvw;
        uvw.build_from_w(intersection_info.normal);

        // DEBUG

        std::cout << "ACTUAL ONB: " << std::endl;
        std::cout << uvw.u() << " , " << uvw.v() << " , " << uvw.w() << std::endl;

        std::vector<Vec3D> xyz;
        xyz = build_ONB(intersection_info.normal);
        std::cout << "MY ONB: " << std::endl;
        std::cout << xyz[0] << " , " << xyz[1] << " , " << xyz[2] << std::endl;
        // END DEBUG


        auto scatter_direction = uvw.local(random_cosine_direction());

        // DEBUG
        Vec3D my_scatter_direction = global_to_ONB_local(xyz, cosine_weighted_direction());
        std::cout << "ACTUAL SCATTER DIRECTION: " << scatter_direction << std::endl;
        std::cout << "MY SCATTER DIRECTION: " << my_scatter_direction << std::endl;

        // END DEBUG
        scattered_ray = Ray(intersection_info.p, unit_vector(scatter_direction), incident_ray.get_time());
        shading_color = surface_color;
        pdf = dot_product(uvw.w(), scattered_ray.get_ray_direction()) / pi;
        return true;
        */
        /* ONB */


       // std::vector<Vec3D> uvw;
       // uvw = build_ONB(intersection_info.normal);
        surface_pdf_ptr = std::make_shared<Cosine_Weighted_PDF>(intersection_info.normal);
        Vec3D scatter_direction = surface_pdf_ptr->generate_a_random_direction_based_on_PDF();
        scattered_ray = Ray(intersection_info.p, unit_vector(scatter_direction), incident_ray.get_time());
        pdf = surface_pdf_ptr->PDF_value(scattered_ray.get_ray_direction());
        shading_color = surface_color;
        return true;

        /* REGULAR - OLD working */

        /*
        material_type = DIFFUSE;
        Vec3D reflection_direction = diffuse_reflection_direction(intersection_info.normal);
        scattered_ray = Ray(intersection_info.p, reflection_direction);
        shading_color = surface_color;
        pdf = this->pdf(incident_ray, intersection_info, scattered_ray);
        return true;*/
    }

    /// Reference: xxx
    double pdf(const Ray &incident_ray, const Intersection_Information &intersection_info, const Ray &scattered_ray) const override {
        auto cos_theta = dot_product(intersection_info.normal, unit_vector(scattered_ray.get_ray_direction()));
        return cos_theta < 0 ? 0 : cos_theta/pi;
    }

    Vec3D BRDF(const Ray &incident_ray, const Intersection_Information &intersection_information, const Ray &scattered_ray, Color& attenuated_color) const override {
        auto cos_theta = dot_product(intersection_information.normal, unit_vector(scattered_ray.get_ray_direction()));
        return cos_theta < 0 ? Vec3D(0,0,0) : attenuated_color * cos_theta/M_PI;
    }
private:
    // Data Members
    Color surface_color;          // = surface color = diffuse coefficient = diffuse color
};

#endif //CUDA_RAY_TRACER_DIFFUSE_H
