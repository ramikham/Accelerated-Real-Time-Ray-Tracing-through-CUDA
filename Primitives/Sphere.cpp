//
// Created by Rami on 9/20/2023.
//

#include "Sphere.h"

/// Reference: [1]
/// ----------------------------------------------------------------------
bool Sphere::intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const {
    // Get the A, B, C of the quadratic equation
    Vec3D OC = r.get_ray_origin() - center;
    auto A = r.get_ray_direction().length_squared();
    auto half_B = dot_product(OC, r.get_ray_direction());           // half_B is a shortcut
    auto C = OC.length_squared() - radius*radius;

    // Calculate the quadratic equation discriminant.
    auto discriminant = half_B * half_B - A * C;

    // If the discriminant is negative, the ray misses the sphere.
    if (discriminant < 0) return false;

    // Calculate the square root of the discriminant.
    double sqrt_discriminant = sqrt(discriminant);

    // Since t > 0 is part of the ray definition, we examine the two
    // roots. The smaller, positive real root is the one that is closest
    // to the intersection distance on the ray.
    double intersection_t = (-half_B - sqrt_discriminant) / A;       // first root
    if (intersection_t <= t_0 || t_1 <= intersection_t) {
        // first root not in range [t_0,t_1], so calculate
        // the second root.
        intersection_t = (-half_B + sqrt_discriminant) / A;

        // Check if second root is also not in the range [t_0,t_1].
        if (intersection_t <= t_0 || t_1 <= intersection_t)
            return false;
    }

    // We know the ray intersects the sphere, so we should update the
    // intersection information
    intersection_info.t = intersection_t;
    intersection_info.p = r.at(intersection_t);
    Vec3D outward_normal = (intersection_info.p - center) / radius;
    intersection_info.set_face_normal(r, outward_normal);
    intersection_info.material_type = mat_type;
    intersection_info.surface_color = surface_color;

    return true;
}
