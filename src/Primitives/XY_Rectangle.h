//
// Created by Rami on 1/31/2024.
//

#ifndef CUDA_RAY_TRACER_XY_RECTANGLE_H
#define CUDA_RAY_TRACER_XY_RECTANGLE_H

#include "Primitive.h"

/// Reference: Ray Tracing: The Next Week
class XY_Rectangle : public Primitive {
public:
    // Constructor
    // -----------------------------------------------------------------------
    XY_Rectangle(const point3D &minPoint, const point3D &maxPoint, const std::shared_ptr<Material> &matPtr) : min_point(
            minPoint), max_point(maxPoint), mat_ptr(matPtr) {
        if (minPoint.z() != maxPoint.z()) {
            std::cerr << "XY_RECTANGLE MUST HAVE ITS Z COMPONENTS IN MIN AND MAX EQUAL!\n";
            exit(0);
        }
        z_comp = minPoint.z();
    }

    // Overloaded Function
    // -------------------------------------------------------------------
    bool intersection(const Ray &r, double t_0, double t_1, Intersection_Information &intersection_info) const override {
        // Does the ray intersect the XY_Rectangle?

        double t = (z_comp - r.get_ray_origin().z()) / r.get_ray_direction().z();

        if (t < t_0 || t > t_1)
            return false;

        double x_comp = r.get_ray_origin().x() + t * r.get_ray_direction().x();
        double y_comp = r.get_ray_origin().y() + t * r.get_ray_direction().y();

        if (x_comp < min_point.x() || x_comp > max_point.x() || y_comp < min_point.y() || y_comp > max_point.y())
            return false;

        /*
        intersection_info.p = point3D((x_comp - min_point.x())/(max_point.x() - min_point.x()),
                                      (y_comp - min_point.y())/(max_point.y() - min_point.y()),
                                      z_comp);*/
        intersection_info.t = t;
        Vec3D outward_normal = unit_vector(Vec3D(0, 0, max_point.z() > min_point.z() ? 1 : -1));
        intersection_info.set_face_normal(r, outward_normal);
        intersection_info.mat_ptr = mat_ptr;
        intersection_info.p = r.at(t);

        return true;
    }

    bool has_bounding_box(double time_0, double time_1, AABB &surrounding_AABB) const override {
        // Does the XY_Rectangle have a bounding box?

        surrounding_AABB = AABB(point3D(min_point.x(), min_point.y(), min_point.z() - 0.0001),
                                point3D(max_point.x(), max_point.y(), max_point.z() + 0.0001));
        return true;
    }

    double PDF_value(const point3D &o, const Vec3D &v) const override {
        // Calculate the PDF value, the likelihood of sampling a random direction on the XY_Rectangle

        Intersection_Information intersection_information;
        if (!this->intersection(Ray(o, v), 0.001, infinity, intersection_information))
            return 0;

        auto area = (max_point.x()-min_point.x())*(max_point.y()-min_point.y());
        auto distance_squared = intersection_information.t * intersection_information.t * v.length_squared();
        auto cosine = fabs(dot_product(v, intersection_information.normal) / v.length());

        return distance_squared / (cosine * area);

    }

    Vec3D random(const Vec3D &o) const override {
        // Generates a random direction within the XY_Rectangle based on importance sampling

        point3D random_point = point3D(random_double(min_point.x(),max_point.x()), random_double(min_point.y(),max_point.y()), z_comp);
        return random_point - o;
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    point3D min_point;
    point3D max_point;
    double z_comp;
    std::shared_ptr<Material> mat_ptr;
};

#endif //CUDA_RAY_TRACER_XY_RECTANGLE_H
