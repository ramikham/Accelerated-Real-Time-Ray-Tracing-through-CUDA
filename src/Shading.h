//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SHADING_H
#define CUDA_RAY_TRACER_SHADING_H

#include "Utilities.h"
#include "Primitives/Primitives_Group.h"
#include "Materials/Diffuse.h"

Color shade(const Ray& r, const Primitive& world, int depth= 10){
    Intersection_Information rec;
    if (depth <= 0)
        return Color(0,0,0);

    if (world.intersection(r,0.001, infinity, rec)) {
        Ray scattered_ray;
        Color visible_color;

        if (rec.mat_ptr->illumination(r, rec, visible_color, scattered_ray)) {
          // std::cout << visible_color;
           return visible_color * shade(scattered_ray, world, depth-1) ;
        }

        return Color(0,0,0);
    }

    // Background color when there is no intersection
    Vec3D unit_direction = unit_vector(r.get_ray_direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);     // for a darker ambient color, use Color(0.2, 0.3, 0.5);
}

#endif //CUDA_RAY_TRACER_SHADING_H
