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

    // Overloaded Functions
    // -----------------------------------------------------------------------
    bool intersection(const Ray &r, double t_min, double t_max, Intersection_Information &intersection_info) const override  {
        // Tests if the ray r intersects the sphere between the interval [t_min,t_max]
        // NOTE: When measuring runtime, don't call this function; instead, paste the intersection code here.

        return ray_sphere_intersection_geometric_solution(r, t_min, t_max, intersection_info);

        // return ray_sphere_intersection_algebraic_solution(r, t_min, t_max, intersection_info);
    }

    /// Reference: An Introduction to Ray Tracing - Section 2.1: Intersection of the Sphere
    bool ray_sphere_intersection_algebraic_solution(const Ray &r, double t_min, double t_max, Intersection_Information &intersection_info) const {
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
        if (intersection_t <= t_min || t_max <= intersection_t) {
            // first root not in range [t_0,t_1], so calculate
            // the second root.
            intersection_t = (-half_B + sqrt_discriminant) / A;

            // Check if second root is also not in the range [t_0,t_1].
            if (intersection_t <= t_min || t_max <= intersection_t)
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

    /// Reference: An Introduction to Ray Tracing - Section 2.1: Intersection of the Sphere
    bool ray_sphere_intersection_geometric_solution(const Ray &r, double t_min, double t_max, Intersection_Information &intersection_info) const {
        bool flag_outside_sphere = false;
        double R2 = radius*radius;
        Vec3D OC  = center - r.get_ray_origin();
        double L_2_OC = dot_product(OC, OC);
        flag_outside_sphere = false;
        if (L_2_OC >= R2) {
            // Ray originated outside the sphere
            flag_outside_sphere = true;
        }

        double t_ca = dot_product(OC, unit_vector(r.get_ray_direction()));

        if (t_ca < 0) {
            if (flag_outside_sphere)
                return false;
        }
        double t_2_hc = R2 - L_2_OC + (t_ca * t_ca);

        if (t_2_hc < 0) {
            return false;
        }

        //  std::cout << "1. " <<  (t_ca - sqrt(t_2_hc)) / r.get_ray_direction().length() << std::endl;
        //  std::cout << "2. " << (t_ca + sqrt(t_2_hc)) / r.get_ray_direction().length() << std::endl;

        double intersection_t;

        if (flag_outside_sphere)
            intersection_t =  (t_ca - sqrt(t_2_hc)) / r.get_ray_direction().length();
        else
            intersection_t = (t_ca + sqrt(t_2_hc)) / r.get_ray_direction().length();

        if (intersection_t <= t_min || t_max <= intersection_t) {
            return false;
        }

        // Intersection exists
        intersection_info.t = intersection_t;
        intersection_info.p = r.at(intersection_t);
        Vec3D outward_normal = (intersection_info.p - center) / radius;
        intersection_info.set_face_normal(r, outward_normal);
        intersection_info.mat_ptr = sphere_material;

        return true;
    }

    /// Reference: https://gamedev.stackexchange.com/questions/159710/getting-the-bounding-box-of-a-sphere
    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        // Construct the sphere's boundign box

        surrounding_AABB = AABB(
                center - Vec3D(radius, radius, radius),
                center + Vec3D(radius, radius, radius)
        );

        return true;
    }

    /// Reference: [Monte Carlo Techniques for Direct Lighting], better explained here:
    ///            - https://schuttejoe.github.io/post/arealightsampling/
    double PDF_value(const point3D &o, const Vec3D &v) const override {
        // Calculate the PDF value, the likelihood of sampling a random direction on the sphere

        Intersection_Information intersection_information;

        if (!this->intersection(Ray(o,v), 0.001, infinity, intersection_information))
            return 0;

        double cos_theta_max = sqrt(1 - (radius * radius)/(center - o).length_squared());
        auto solid_angle = 2*M_PI*(1-cos_theta_max);
        return 1 / solid_angle;
    }

    Vec3D random(const Vec3D &o) const override {
        // Generates a random direction within the sphere based on importance sampling

        Vec3D direction = center - o;
        auto distance_squared = direction.length_squared();
        std::vector<Vec3D> uvw = build_ONB(direction);
        return global_to_ONB_local(uvw, importance_sampling_sphere(radius, distance_squared));
    }

public:
    static Vec3D importance_sampling_sphere(double radius, double distance_squared) {
        // Generates a random 3D point on the surface of a sphere using importance sampling,
        // biased towards regions that contribute more to the overall result (i.e., those that
        // face the light, rather than the ones that are at the other side)

        double r1 = random_double();
        double r2 = random_double();
        double z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

        double phi = 2*M_PI*r1;
        double x = cos(phi)*sqrt(1-z*z);
        double y = sin(phi)*sqrt(1-z*z);

        return Vec3D(x, y, z);
    }

    // Data Members
    // -----------------------------------------------------------------------
    point3D center;                                     // center of the sphere
    double radius;                                      // radius of the sphere
    std::shared_ptr<Material> sphere_material;          // material of the sphere
};

#endif //CUDA_RAY_TRACER_SPHERE_H
