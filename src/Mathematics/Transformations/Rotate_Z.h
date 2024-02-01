//
// Created by Rami on 1/29/2024.
//

#ifndef CUDA_RAY_TRACER_ROTATE_Z_H
#define CUDA_RAY_TRACER_ROTATE_Z_H

#include "../../Primitives/Primitive.h"

class Rotate_Z : public Primitive {
public:
    Rotate_Z(std::shared_ptr<Primitive> primitive_ptr, double rotation_angle)
            : primitive_ptr(primitive_ptr)
    {
        double radians = degrees_to_radians(rotation_angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        has_bbox = primitive_ptr->has_bounding_box(0, 1, bbox);

        point3D min(infinity, infinity, infinity);
        point3D max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * bbox.get_max().x() + (1 - i) * bbox.get_min().x();
                    auto y = j * bbox.get_max().y() + (1 - j) * bbox.get_min().y();
                    auto z = k * bbox.get_max().z() + (1 - k) * bbox.get_min().z();

                    auto newx = cos_theta * x - sin_theta * y;
                    auto newy = sin_theta * x + cos_theta * y;

                    Vec3D tester(newx, newy, z);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }

    bool intersection(const Ray& r, double t_0, double t_1, Intersection_Information& intersection_info) const override
    {
        point3D origin = r.get_ray_origin();
        Vec3D direction = r.get_ray_direction();

        origin[0] = cos_theta * r.get_ray_origin()[0] - sin_theta * r.get_ray_origin()[1];
        origin[1] = sin_theta * r.get_ray_origin()[0] + cos_theta * r.get_ray_origin()[1];

        direction[0] = cos_theta * r.get_ray_direction()[0] - sin_theta * r.get_ray_direction()[1];
        direction[1] = sin_theta * r.get_ray_direction()[0] + cos_theta * r.get_ray_direction()[1];

        Ray rotated_ray(origin, direction, r.get_time());

        if (!primitive_ptr->intersection(rotated_ray, t_0, t_1, intersection_info))
            return false;

        point3D p = intersection_info.p;
        Vec3D normal = intersection_info.normal;

        p[0] = cos_theta * intersection_info.p[0] - sin_theta * intersection_info.p[1];
        p[1] = sin_theta * intersection_info.p[0] + cos_theta * intersection_info.p[1];

        normal[0] = cos_theta * intersection_info.normal[0] - sin_theta * intersection_info.normal[1];
        normal[1] = sin_theta * intersection_info.normal[0] + cos_theta * intersection_info.normal[1];

        intersection_info.p = p;
        intersection_info.normal = normal;

        return true;
    }

    bool has_bounding_box(double time_0, double time_1, AABB& surrounding_AABB) const override
    {
        surrounding_AABB = bbox;
        return true;
    }

private:
    std::shared_ptr<Primitive> primitive_ptr;
    double sin_theta;
    double cos_theta;
    bool has_bbox;
    AABB bbox;
};

#endif //CUDA_RAY_TRACER_ROTATE_Z_H
