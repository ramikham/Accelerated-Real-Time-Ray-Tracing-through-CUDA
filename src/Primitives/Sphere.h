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
    bool has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const override;

public:
    point3D center;                                     // center of the sphere
    double radius;                                      // radius of the sphere
    std::shared_ptr<Material> sphere_material;          // material of the sphere
};

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
    intersection_info.mat_ptr = sphere_material;

    return true;
}

bool Sphere::has_bounding_box(double time_0, double time_1, Axis_Aligned_Bounding_Box &surrounding_AABB) const {
    // Construct the sphere's AABB
    //return false;
    //  std::cout << "constructed";
    surrounding_AABB = Axis_Aligned_Bounding_Box(
            center - Vec3D(radius, radius, radius),
            center + Vec3D(radius, radius, radius)
    );

    return true;
}

#endif //CUDA_RAY_TRACER_SPHERE_H
