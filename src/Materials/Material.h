//
// Created by Rami on 10/7/2023.
//

#ifndef CUDA_RAY_TRACER_MATERIAL_H
#define CUDA_RAY_TRACER_MATERIAL_H

#include "../Utilities.h"
#include "../Primitives/Primitive.h"
#include "../Mathematics/Probability/PDF.h"

struct Intersection_Information;            // predef

enum MATERIAL_TYPE {
    SPECULAR = 111,
    PHONG = 1600
};

/// References:     - Fundamentals of Computer Graphics - Section 4.5.1: Light Sources
///                 - Fundamentals of Computer Graphics - Section 4.5.2: Shading in Software
class Material {
public:
    virtual ~Material()=default;

    // Evaluates the shading model
    virtual bool evaluate(const Ray& incident_ray, const Intersection_Information& intersection_info, Color& shading_color, Ray& scattered_ray, MATERIAL_TYPE& , double& pdf, std::shared_ptr<PDF>& surface_pdf_ptr) const = 0;

    virtual double pdf(const Ray& incident_ray, const Intersection_Information& intersection_info, const Ray& scattered_ray) const {
        return 0;
    }
    virtual Vec3D BRDF(const Ray& incident_ray, const Intersection_Information& intersection_information, const Ray& scattered_ray, Color& attenuated_color) const {
        return Vec3D(1,1,1);
    }

    /// Reference: Fundamentals of Computer Graphics - Section 14.10
    virtual Color emitted(const point3D& p, const Intersection_Information& intersection_information) {
        return Color(0,0,0);
    }
};
#endif //CUDA_RAY_TRACER_MATERIAL_H
