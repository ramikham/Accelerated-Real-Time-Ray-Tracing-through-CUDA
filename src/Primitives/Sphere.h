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
    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override;

    double PDF_value(const point3D &o, const Vec3D &v) const override {
        Intersection_Information intersection_information;

        if (!this->intersection(Ray(o,v), 0.001, infinity, intersection_information))
            return 0;

        double cos_theta_max = sqrt(1 - (radius * radius)/(center - o).length_squared());
        auto solid_angle = 2*M_PI*(1-cos_theta_max);
        return 1 / solid_angle;
    }

    Vec3D random(const Vec3D &o) const override {
        Vec3D direction = center - o;
        auto distance_squared = direction.length_squared();
        std::vector<Vec3D> uvw = build_ONB(direction);
        return global_to_ONB_local(uvw, importance_sampling_sphere(radius, distance_squared));
    }

public:
    static Vec3D importance_sampling_sphere(double radius, double distance_squared) {
        double r1 = random_double();
        double r2 = random_double();
        double z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

        double phi = 2*M_PI*r1;
        double x = cos(phi)*sqrt(1-z*z);
        double y = sin(phi)*sqrt(1-z*z);

        return Vec3D(x, y, z);
    }

    point3D center;                                     // center of the sphere
    double radius;                                      // radius of the sphere
    std::shared_ptr<Material> sphere_material;          // material of the sphere
};

/// Reference: An Introduction to Ray Tracing - Section 2.1: Intersection of the Sphere
bool Sphere::intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const {
    // Tests if the ray r intersects the sphere between the interval [t_0,t_1]

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

/// Reference: xxx
bool Sphere::has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const {
    // Construct the sphere's AABB

    surrounding_AABB = AABB(
            center - Vec3D(radius, radius, radius),
            center + Vec3D(radius, radius, radius)
    );

    return true;
}

#endif //CUDA_RAY_TRACER_SPHERE_H
