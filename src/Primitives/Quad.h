//
// Created by Rami on 1/28/2024.
//

#ifndef CUDA_RAY_TRACER_QUAD_H
#define CUDA_RAY_TRACER_QUAD_H

#include "Primitive.h"

/// Reference: Ray Tracing: The Next Week
// https://raw.githubusercontent.com/RayTracing/raytracing.github.io/release/src/TheNextWeek/quad.h
// I am currently working on my own implementation for this class...
class Quad : public Primitive {
public:
    Quad(const point3D& Q, const Vec3D& u, const Vec3D v, std::shared_ptr<Material> mat_ptr)
        : Q(Q), u(u), v(v), mat_ptr(mat_ptr) {
        auto n = cross_product(u, v);
        normal = unit_vector(n);
        D = dot_product(normal, Q);
        w = n / dot_product(n,n);

        area = n.length();

        bbox = AABB(Q, Q + u + v).AABB();
    }

    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        auto denom = dot_product(normal, r.get_ray_direction());

        // If the ray is parallel to the plane, then there is not intersection
        if (fabs(denom) < EPSILON)
            return false;

        // Return false if the intersection point parameter t is outside the ray interval
        auto intersection_t = (D - dot_product(normal, r.get_ray_origin())) / denom;
        if (intersection_t <= t_0 || t_1 <= intersection_t)
            return false;

        // Determine whether the intersection point lies within the planar shape using its plane coordiantes
        auto intersection = r.at(intersection_t);
        Vec3D planar_hitpoint_vector = intersection - Q;
        auto alpha = dot_product(w, cross_product(planar_hitpoint_vector, v));
        auto beta = dot_product(w, cross_product(u, planar_hitpoint_vector));

        if (!is_interior(alpha, beta, intersection_info))
            return false;

        // We know the ray intersects the quad, so we should update the
        // intersection information
        intersection_info.t = intersection_t;
        intersection_info.p = intersection;
        intersection_info.mat_ptr = mat_ptr;
        intersection_info.set_face_normal(r, normal);

        return true;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        surrounding_AABB = bbox;

        return true;
    }

    double PDF_value(const point3D& o, const Vec3D& v) const override {
        Intersection_Information rec;
        if (!this->intersection(Ray(o, v), 0.001, infinity, rec))
            return 0;

        double distance_squared = rec.t * rec.t * v.length_squared();
        double cosine = fabs(dot_product(v, rec.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    Vec3D random(const Vec3D &o) const override {
        Vec3D p = Q + (random_double() * u) + (random_double() * v);
        return p - o;
    }

private:
    bool is_interior(double a, double b, Intersection_Information& intersection_information) const {
        if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        // TODO: UPDATE FOR TEXTURES LATER ON
        return true;
    }
    point3D Q;
    Vec3D u;
    Vec3D v;
    std::shared_ptr<Material> mat_ptr;
    AABB bbox;
    Vec3D normal;
    double D;
    Vec3D w;
    double area;
};

#endif //CUDA_RAY_TRACER_QUAD_H
