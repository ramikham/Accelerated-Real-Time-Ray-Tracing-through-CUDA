//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SHADING_H
#define CUDA_RAY_TRACER_SHADING_H

#include "Utilities.h"
#include "Primitives/Primitives_Group.h"
#include "Materials/Diffuse.h"
#include "Mathematics/Probability/PDF.h"
#include "Mathematics/Probability/Primitive_PDF.h"
#include "Mathematics/Probability/Mixture_PDF.h"

/// Reference: xxx
/*
Color shade(const Ray& r, const Primitive& world, int depth= 10){
    Intersection_Information rec;
    if (depth <= 0)
        return Color(0,0,0);

    if (world.intersection(r,0.001, infinity, rec)) {
        Ray scattered_ray;
        Color visible_color;
        MATERIAL_TYPE material_type;

        if (rec.mat_ptr->illumination(r, rec, visible_color, scattered_ray, material_type)) {
          // std::cout << visible_color;
           return visible_color * shade(scattered_ray, world, depth-1);
        }

        return Color(0,0,0);
    }

    // Background color when there is no intersection
    Vec3D unit_direction = unit_vector(r.get_ray_direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);

    // for a darker ambient color, use Color(0.2, 0.3, 0.5); for brighter use Color(0.5, 0.7, 1.0)
    //return (1.0 - a) * Color(0.0, 0.0, 0.0) + a * Color(0.1, 0.15, 0.2); // dark space
    return (1.0-a)* Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
}*/

Color radiance(const Ray& r, const Primitive& world, int depth= 10){
    Intersection_Information rec;
    if (depth <= 0)
        return Color(0,0,0);

    if (!world.intersection(r, 0.001, infinity, rec)) {
        // Background color when there is no intersection
        Vec3D unit_direction = unit_vector(r.get_ray_direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);

        // for a darker ambient color, use Color(0.2, 0.3, 0.5); for brighter use Color(0.5, 0.7, 1.0)
        //return (1.0 - a) * Color(0.0, 0.0, 0.0) + a * Color(0.1, 0.15, 0.2); // dark space
        return (1.0-a)* Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
    }
    Ray scattered_ray;
    Color surface_color;
    MATERIAL_TYPE material_type;
    std::shared_ptr<PDF> surface_pdf_ptr;
    double pdf;

    // std::cout << "Normal from shade(): " << rec.normal << std::endl;
    if (!rec.mat_ptr->illumination(r, rec, surface_color, scattered_ray, material_type, pdf, surface_pdf_ptr))
        return Color(1.0,1.0,1.0);

    return rec.mat_ptr->BRDF(r, rec, scattered_ray, surface_color) *
           radiance(scattered_ray, world, depth-1) / pdf;
}


// Color(0.70, 0.80, 1.00) is sky blue
// (0,0,0) is dark
// (0.04, 0.04, 0.08) sky dark
Color radiance_background(const Ray& r, const Primitive& world, int depth= 10, Color background=Color(0,0,0)){
    Intersection_Information rec;
    if (depth <= 0)
        return Color(0,0,0);

    if (!world.intersection(r, 0.001, infinity, rec))
        // Background color when there is no intersection
        return background;

    Ray scattered_ray;
    Color surface_color;
    MATERIAL_TYPE material_type;
    std::shared_ptr<PDF> surface_pdf_ptr;
    double pdf;

    Color color_from_emission = rec.mat_ptr->emitted(rec.p, rec);

    if (!rec.mat_ptr->illumination(r, rec, surface_color, scattered_ray, material_type, pdf, surface_pdf_ptr))
        return color_from_emission;

  //  std::cout << "Color from scatter = " << rec.mat_ptr->BRDF(r, rec, scattered_ray, surface_color) << std::endl;
    return color_from_emission + rec.mat_ptr->BRDF(r, rec, scattered_ray, surface_color) *
                                 radiance_background(scattered_ray, world, depth-1, background) / pdf;
}

Color radiance_sample_light_directly(const Ray& r, const Primitive& world, int depth= 10, Color background=Color(0,0,0)){
    Intersection_Information rec;
    if (depth <= 0)
        return Color(0,0,0);

    if (!world.intersection(r, 0.001, infinity, rec))
        // Background color when there is no intersection
        return background;

    Ray scattered_ray;
    Color surface_color;
    MATERIAL_TYPE material_type;
    std::shared_ptr<PDF> surface_pdf_ptr;
    double pdf;

    // SAMPLE LIGHT DIRECTLY
    Color color_from_emission = rec.mat_ptr->emitted(rec.p, rec);

    if (!rec.mat_ptr->illumination(r, rec, surface_color, scattered_ray, material_type, pdf, surface_pdf_ptr))
        return color_from_emission;

    point3D on_light = point3D(random_double(213, 343), 554, random_double(227, 332));
    Vec3D to_light = on_light - rec.p;
    auto distance_squared = to_light.length_squared();
    to_light = unit_vector(to_light);

    if (dot_product(to_light, rec.normal) < 0)
        return color_from_emission;

    double light_area = (343-213)*(332-227);
    auto light_cosine = fabs(to_light.y());
    if (light_cosine < 0.000001)
        return color_from_emission;

    pdf = distance_squared / (light_cosine * light_area);
    scattered_ray = Ray(rec.p, to_light, r.get_time());

    return color_from_emission + rec.mat_ptr->BRDF(r, rec, scattered_ray, surface_color) *
                                 radiance_sample_light_directly(scattered_ray, world, depth-1, background) / pdf;
}

Color radiance_mixture(const Ray& r, const Primitive& world, const Primitive& lights, int depth= 10, Color background=Color(0,0,0)){
    Intersection_Information rec;
    if (depth <= 0)
        return Color(0,0,0);

    if (!world.intersection(r, 0.001, infinity, rec))
        // Background color when there is no intersection
        return background;

    Ray scattered_ray;
    Color surface_color;
    MATERIAL_TYPE material_type;
    std::shared_ptr<PDF> surface_pdf_ptr;
    double pdf;

    Color color_from_emission = rec.mat_ptr->emitted(rec.p, rec);

    if (!rec.mat_ptr->illumination(r, rec, surface_color, scattered_ray, material_type, pdf, surface_pdf_ptr))
        return color_from_emission;

    auto light_ptr = std::make_shared<Primitive_PDF>(lights, rec.p);
    Mixture_PDF mixture_pdf(light_ptr, surface_pdf_ptr);

  // std::cout << mixture_pdf.generate_a_random_direction_based_on_PDF() << std::endl;
    scattered_ray = Ray(rec.p, mixture_pdf.generate_a_random_direction_based_on_PDF(), r.get_time());
    double new_pdf = mixture_pdf.PDF_value(scattered_ray.get_ray_direction());

    return color_from_emission + rec.mat_ptr->BRDF(r, rec, scattered_ray, surface_color) *
                                 radiance_mixture(scattered_ray, world, lights, depth-1, background) / new_pdf;
}

/*
Color ray_color(const Ray& r, const Primitive& world, int depth= 10) {
    Intersection_Information rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return Color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.intersection(r, 0.001, infinity, rec)) {
        // Background color when there is no intersection
        Vec3D unit_direction = unit_vector(r.get_ray_direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);

        // for a darker ambient color, use Color(0.2, 0.3, 0.5); for brighter use Color(0.5, 0.7, 1.0)
        //return (1.0 - a) * Color(0.0, 0.0, 0.0) + a * Color(0.1, 0.15, 0.2); // dark space
        return (1.0-a)* Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
    }


    Ray scattered;
    Color attenuation;
//    Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
    MATERIAL_TYPE material_type;

    if (!rec.mat_ptr->illumination(r, rec, attenuation, scattered, material_type)) {
        // Background color when there is no intersection
        Vec3D unit_direction = unit_vector(r.get_ray_direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);

        // for a darker ambient color, use Color(0.2, 0.3, 0.5); for brighter use Color(0.5, 0.7, 1.0)
        //return (1.0 - a) * Color(0.0, 0.0, 0.0) + a * Color(0.1, 0.15, 0.2); // dark space
        return (1.0-a)* Color(1.0, 1.0, 1.0) + a*Color(0.5, 0.7, 1.0);
    }

    double scattering_pdf = rec.mat_ptr->pdf(r, rec, scattered);
    double pdf =  1/(2*M_PI);

    Color color_from_scatter =
            (attenuation * scattering_pdf * ray_color(scattered, world, depth-1)) / pdf;

    return color_from_scatter;
}*/
#endif //CUDA_RAY_TRACER_SHADING_H
