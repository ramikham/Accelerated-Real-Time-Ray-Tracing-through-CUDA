//
// Created by Rami on 1/31/2024.
//

#ifndef CUDA_RAY_TRACER_XZ_RECTANGLE_H
#define CUDA_RAY_TRACER_XZ_RECTANGLE_H

#include "Primitive.h"

/// Reference: Ray Tracing: The Next Week
class XZ_Rectangle : public Primitive {
public:
    XZ_Rectangle(const point3D &minPoint, const point3D &maxPoint, const std::shared_ptr<Material> &matPtr)
        : min_point(minPoint), max_point(maxPoint), mat_ptr(matPtr) {
        y_comp = minPoint.y();
        area = (max_point.x() - min_point.x()) * (max_point.z() - min_point.z());
    }

    bool
    intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {

        double t = (y_comp - r.get_ray_origin().y()) / r.get_ray_direction().y();

        if (t < t_0 || t > t_1)
            return false;

        double x_comp = r.get_ray_origin().x() + t * r.get_ray_direction().x();
        double z_comp = r.get_ray_origin().z() + t * r.get_ray_direction().z();

        if (x_comp < min_point.x() || x_comp > max_point.x() || z_comp < min_point.z() || z_comp > max_point.z())
            return false;

        intersection_info.t = t;
        Vec3D outward_normal = unit_vector(cross_product(Vec3D(min_point.x() - max_point.x(),0,0), Vec3D(0,0,min_point.z() - max_point.z())));
        intersection_info.set_face_normal(r, outward_normal);
        intersection_info.mat_ptr = mat_ptr;
        intersection_info.p = r.at(t);

        return true;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        surrounding_AABB = AABB(point3D(min_point.x(), y_comp - 0.0001, min_point.z()),
                                point3D(max_point.x(), y_comp + 0.0001, max_point.z()));
        return true;
    }

    double PDF_value(const point3D &o, const Vec3D &v) const override {
        Intersection_Information intersection_info;
        if (!this->intersection(Ray(o, v), 0.001, infinity, intersection_info))
            return 0;

        auto distance_squared = intersection_info.t * intersection_info.t * v.length_squared();
        auto cosine = fabs(dot_product(v, intersection_info.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    Vec3D random(const Vec3D &o) const override {
        point3D random_point = point3D(random_double(min_point.x(), max_point.x()), y_comp,
                                       random_double(min_point.z(), max_point.z()));
        return random_point - o;
    }

    // Getters
    // -------------------------------------------------------------------------------
    double get_area() const {
        return area;
    }

private:
    point3D min_point;
    point3D max_point;
    double y_comp;
    double area;
    std::shared_ptr<Material> mat_ptr;
};

#endif //CUDA_RAY_TRACER_XZ_RECTANGLE_H
