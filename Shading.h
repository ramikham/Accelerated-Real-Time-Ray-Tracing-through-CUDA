//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SHADING_H
#define CUDA_RAY_TRACER_SHADING_H

#include "Utilities.h"
#include "Primitives/Primitives_Group.h"
Color cast_ray(const Ray& r, const Primitive& world, int depth=10){
    Intersection_Information rec;

    if (depth <= 0)
        return Color(0,0,0);

    if (world.intersection(r,0.001, infinity, rec)) {
        // Calculate the Lambertian reflection term
        Ray reflected_ray;
        Color visible_color;
        // Vec3D light_direction = random_unit_vector() - rec.normal;
        if (rec.material_type == DIFFUSE) {
            // Calculate the direction of the reflected ray
            auto R = diffuse_reflection_direction(rec.normal);

            // Initialize the reflected ray
            reflected_ray  = Ray(rec.p, R);

            // A diffuse material reflects the colors we see on its surface
            visible_color = rec.surface_color;
            return visible_color * cast_ray_v3(reflected_ray, world, depth - 1);
        }

        else if (rec.material_type == SPECULAR) {
            // Calculate the direction of the reflected vector
            Vec3D R = specular_reflection_direction(unit_vector(r.get_ray_direction()), rec.normal);

            // If R happens to be exactly opposite to our N, we will get a 0 direction.
            // Let us avoid that:
            if (R.x() < 1e-8 && R.y() < 1e-8 && R.z() < 1e-8)
                R = rec.normal;

            // Initialize the reflect ray
            reflected_ray = Ray(rec.p, R);

            // A specular material (mirror) reflects the same color
            visible_color = rec.surface_color;
            return visible_color * cast_ray_v3(reflected_ray, world, depth - 1);
        }
        return Color(0,0,0);
    }

    // Background color when there is no intersection
    Vec3D unit_direction = unit_vector(r.get_ray_direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

#endif //CUDA_RAY_TRACER_SHADING_H
