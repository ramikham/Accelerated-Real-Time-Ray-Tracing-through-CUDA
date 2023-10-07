//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_SPHERE_H
#define CUDA_RAY_TRACER_SPHERE_H

#include "Primitive.h"
class Sphere : public Primitive {
public:
    // Constructor
    // -----------------------------------------------------------------------
    Sphere(point3D center, double radius, std::shared_ptr<Material> material) :
    center(center), radius(radius), sphere_material(material) {}

    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override;

public:
    point3D center;                                     // center of the sphere
    double radius;                                      // radius of the sphere
    std::shared_ptr<Material> sphere_material;          // material of the sphere
};

#endif //CUDA_RAY_TRACER_SPHERE_H
