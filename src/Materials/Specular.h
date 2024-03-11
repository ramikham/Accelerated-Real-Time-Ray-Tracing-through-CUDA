//
// Created by Rami on 10/10/2023.
//

#ifndef CUDA_RAY_TRACER_SPECULAR_H
#define CUDA_RAY_TRACER_SPECULAR_H

#include "Material.h"
#include "../Mathematics/Probability/Uniform_Hemispherical_PDF.h"

class Specular : public Material {
public:
    Specular(const Color& surface_color, double glossy_reflection_fraction, double index_of_refraction) : surface_color(
            surface_color), glossy_reflection_fraction(glossy_reflection_fraction), index_of_refraction(
            index_of_refraction) {
        if (glossy_reflection_fraction > 0) {
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

    /// References:             - Fundamentals of Computer Graphics: Section 4.5.4: Mirror Reflection
    ///                         - Fundamentals of Computer Graphics: Section 14.2: Smooth Metals
    ///                         - Fundamentals of Computer Graphics: Section 14.3.1: Reflectivity of a Dielectric
    ///                         - Fundamentals of Computer Graphics: Section 14.3.2: Refraction
    bool evaluate(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                  Ray &scattered_ray, MATERIAL_TYPE& material_type, double& pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const override {
        if (reflection) {
            surface_pdf_ptr = nullptr;
            material_type = SPECULAR;

            // Calculate the direction of the reflected vector
            Vec3D R = specular_reflection_direction(unit_vector(incident_ray.get_ray_direction()),
                                                    intersection_info.normal);

            // Enable glossy reflection (mirror: glossy_reflection_fraction = 0)
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
    bool reflection;                            // does a reflection happen?
    bool refraction;                            // does a refraction happen?
    Color surface_color;                        // color of the specular material
    double glossy_reflection_fraction;          // the fraction of glossiness
    double index_of_refraction;                 // refraction index of the medium

    /* Examples of indices of refraction
     * air: 1.00
     * water: 1.33–1.34
     * window glass: 1.51
     * optical glass: 1.49–1.92
     * diamond: 2.42
     */
};

#endif //CUDA_RAY_TRACER_SPECULAR_H
