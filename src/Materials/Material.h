//
// Created by Rami on 10/7/2023.
//

#ifndef CUDA_RAY_TRACER_MATERIAL_H
#define CUDA_RAY_TRACER_MATERIAL_H

#include "../Utilities.h"
#include "../Primitives/Primitive.h"

// predef
struct Intersection_Information;

class Material {
public:
    virtual ~Material()=default;
    virtual bool illumination(
            const Ray& incident_ray, const Intersection_Information& intersection_info,
            Color& shading_color, Ray& scattered_ray) const = 0;
};
#endif //CUDA_RAY_TRACER_MATERIAL_H
