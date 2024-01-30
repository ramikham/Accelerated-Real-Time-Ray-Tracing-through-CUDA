//
// Created by Rami on 1/28/2024.
//

#ifndef CUDA_RAY_TRACER_DIFFUSE_LIGHT_H
#define CUDA_RAY_TRACER_DIFFUSE_LIGHT_H

#include "Material.h"

class Diffuse_Light : public Material {
public:
    Diffuse_Light(Color light_color) : light_color(light_color) {}

    bool illumination(const Ray &incident_ray, const Intersection_Information &intersection_info, Color &shading_color,
                      Ray &scattered_ray, MATERIAL_TYPE &type, double &pdf) const override {
        return false;
    }

    Color emitted(const point3D &p, const Intersection_Information& intersection_information) override {
        if (!intersection_information.front_face)
            return Color(0,0,0);
        return light_color;
    }

private:
    Color light_color;
};

#endif //CUDA_RAY_TRACER_DIFFUSE_LIGHT_H
