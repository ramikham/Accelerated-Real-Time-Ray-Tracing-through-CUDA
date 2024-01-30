//
// Created by Rami on 1/25/2024.
//

#ifndef CUDA_RAY_TRACER_TRANSLATE_H
#define CUDA_RAY_TRACER_TRANSLATE_H

#include "Primitives/Primitive.h"

class translate : public Primitive {
public:
    translate(std::shared_ptr<Primitive> p, const Vec3D& displacement)
            : ptr(p), offset(displacement) {}

    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override;

    bool has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const override;
public:
    std::shared_ptr<Primitive> ptr;
    Vec3D offset;
};

bool translate::intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const {
    Ray moved_r(r.get_ray_origin() - offset, r.get_ray_direction(), r.get_time());
    if (!ptr->intersection(moved_r, t_0, t_1, intersection_info))
        return false;
    intersection_info.p += offset;
    intersection_info.set_face_normal(moved_r, intersection_info.normal);

    return true;
}

bool translate::has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const {
    if (!ptr->has_bounding_box(time_0, time_1, surrounding_AABB))
        return false;

    surrounding_AABB = Axis_Aligned_Bounding_Box(
            surrounding_AABB.get_min() + offset,
            surrounding_AABB.get_max() + offset);

    return true;
}

#endif //CUDA_RAY_TRACER_TRANSLATE_H
