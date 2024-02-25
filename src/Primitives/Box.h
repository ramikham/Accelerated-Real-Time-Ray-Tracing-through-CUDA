//
// Created by Rami on 2/25/2024.
//

#ifndef CUDA_RAY_TRACER_BOX_H
#define CUDA_RAY_TRACER_BOX_H

#include "Primitive.h"
#include "Primitives_Group.h"
#include "XZ_Rectangle.h"
#include "XY_Rectangle.h"
#include "YZ_Rectangle.h"

class Box : public Primitive {
public:
    Box() {}

    Box(const Vec3D& min_point, const Vec3D& max_point, const std::shared_ptr<Material>& mat_ptr) :
            min_point(min_point), max_point(max_point), mat_ptr(mat_ptr) {

        box_sides = std::make_shared<Primitives_Group>();

        box_sides->add_primitive_to_list(std::make_shared<XY_Rectangle>(
                min_point,
                Vec3D(max_point.x(), max_point.y(), min_point.z()),
                mat_ptr)
                );

        box_sides->add_primitive_to_list(std::make_shared<XY_Rectangle>(
                Vec3D(min_point.x(), min_point.y(), max_point.z()),
                Vec3D(max_point.x(), max_point.y(), max_point.z()),
                mat_ptr)
                );

        box_sides->add_primitive_to_list(std::make_shared<YZ_Rectangle>(
                min_point,
                Vec3D(min_point.x(), max_point.y(), max_point.z()),
                mat_ptr)
        );

        box_sides->add_primitive_to_list(std::make_shared<YZ_Rectangle>(
                Vec3D(max_point.x(), min_point.y(), min_point.z()),
                Vec3D(max_point.x(), max_point.y(), max_point.z()),
                mat_ptr)
        );

        box_sides->add_primitive_to_list(std::make_shared<XZ_Rectangle>(
                min_point,
                Vec3D(max_point.x(), min_point.y(), max_point.z()),
                mat_ptr)
        );

        box_sides->add_primitive_to_list(std::make_shared<XZ_Rectangle>(
                Vec3D(min_point.x(), max_point.y(), min_point.z()),
                Vec3D(max_point.x(), max_point.y(), max_point.z()),
                mat_ptr)
        );

    }

    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        return box_sides->intersection(r, t_0, t_1, intersection_info);
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        surrounding_AABB = AABB{ min_point, max_point };
        return true;
    }

private:
    point3D min_point;
    point3D max_point;
    std::shared_ptr<Material> mat_ptr;
    std::shared_ptr<Primitives_Group> box_sides;
};

#endif //CUDA_RAY_TRACER_BOX_H
