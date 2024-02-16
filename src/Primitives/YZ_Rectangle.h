//
// Created by Rami on 1/31/2024.
//

#ifndef CUDA_RAY_TRACER_YZ_RECTANGLE_H
#define CUDA_RAY_TRACER_YZ_RECTANGLE_H

#include "Primitive.h"

/// Reference: Ray Tracing: The Next Week
class YZ_Rectangle : public Primitive {
public:
    // Constructor
    // -----------------------------------------------------------------------
    YZ_Rectangle(const point3D &minPoint, const point3D &maxPoint, const std::shared_ptr<Material> &matPtr)
        : min_point(minPoint), max_point(maxPoint), mat_ptr(matPtr) {
        if (minPoint.x() != maxPoint.x()) {
            std::cerr << "YZ_RECTANGLE MUST HAVE ITS X COMPONENTS IN MIN AND MAX EQUAL!\n";
            exit(0);
        }
        x_comp = minPoint.x();
    }

    // Overloaded Function
    // -------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        // Does the ray intersect the YZ_Rectangle?

        double t = (x_comp - r.get_ray_origin().x()) / r.get_ray_direction().x();

        if (t < t_0 || t > t_1)
            return false;

        double y_comp = r.get_ray_origin().y() + t * r.get_ray_direction().y();
        double z_comp = r.get_ray_origin().z() + t * r.get_ray_direction().z();

        if (y_comp < min_point.y() || y_comp > max_point.y() || z_comp < min_point.z() || z_comp > max_point.z())
            return false;

        intersection_info.t = t;
        Vec3D outward_normal = unit_vector(cross_product(Vec3D(0,min_point.y() - max_point.y(),0), Vec3D(0,0,min_point.z() - max_point.z())));
        intersection_info.set_face_normal(r, outward_normal);
        intersection_info.mat_ptr = mat_ptr;
        intersection_info.p = r.at(t);

        return true;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        // Does the YZ_Rectangle have a bounding box?

        surrounding_AABB = AABB(point3D(x_comp - 0.0001, min_point.y(), min_point.z()),
                                point3D(x_comp + 0.0001, max_point.y(), max_point.z()));
        return true;
    }


    double PDF_value(const point3D &o, const Vec3D &v) const override {
        // Calculate the PDF value, the likelihood of sampling a random direction on the YZ_Rectangle

        Intersection_Information intersection_info;
        if (!this->intersection(Ray(o, v), 0.001, infinity, intersection_info))
            return 0;

        auto area = (max_point.y() - min_point.y()) * (max_point.z() - min_point.z());
        auto distance_squared = intersection_info.t * intersection_info.t * v.length_squared();
        auto cosine = fabs(dot_product(v, intersection_info.normal) / v.length());

        return distance_squared / (cosine * area);
    }

    Vec3D random(const Vec3D &o) const override {
        // Generates a random direction within the YZ_Rectangle based on importance sampling

        point3D random_point = point3D(x_comp, random_double(min_point.y(), max_point.y()),
                                       random_double(min_point.z(), max_point.z()));
        return random_point - o;
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    point3D min_point;
    point3D max_point;
    double x_comp;
    std::shared_ptr<Material> mat_ptr;
};

#endif //CUDA_RAY_TRACER_YZ_RECTANGLE_H
