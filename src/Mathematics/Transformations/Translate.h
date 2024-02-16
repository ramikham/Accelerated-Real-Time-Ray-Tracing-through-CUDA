//
// Created by Rami on 1/29/2024.
//

#ifndef CUDA_RAY_TRACER_TRANSLATE_H
#define CUDA_RAY_TRACER_TRANSLATE_H

#include "../../Primitives/Primitive.h"

class Translate : public Primitive {
public:
    Translate(std::shared_ptr<Primitive> primitive_ptr, const Vec3D& displacement) :
    primitive_ptr(primitive_ptr), displacement(displacement) {}

    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        Ray translate_ray(r.get_ray_origin() - displacement, r.get_ray_direction(), r.get_time());
        if (!primitive_ptr->intersection(translate_ray, t_0, t_1, intersection_info))
            return false;

        intersection_info.p += displacement;
        intersection_info.set_face_normal(translate_ray, intersection_info.normal);

        return true;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        // TODO: IMPLEMENT THIS AND CHECK THAT THE CLASS IS CORRECT
        if (!primitive_ptr->has_bounding_box(time_0, time_1, surrounding_AABB))
            return false;

        surrounding_AABB = AABB(
                surrounding_AABB.get_min() + displacement,
                surrounding_AABB.get_max() + displacement
                );

        return true;
    }

public:
    std::shared_ptr<Primitive> primitive_ptr;
    Vec3D displacement;
};

#endif //CUDA_RAY_TRACER_TRANSLATE_H
