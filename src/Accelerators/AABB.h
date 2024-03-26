//
// Created by Rami on 10/17/2023.
//

#ifndef CUDA_RAY_TRACER_AABB_H
#define CUDA_RAY_TRACER_AABB_H

#include "../Utilities.h"

class AABB {
public:
    // Constructors
    // -----------------------------------------------------------------------
    AABB() {}
    AABB(const point3D& MIN, const point3D& MAX) : minimum(MIN), maximum(MAX) {
        bounds[0] = MIN;
        bounds[1] = MAX;

        // Calculate centroid
        centroid = 0.5 * (minimum + maximum);
    }

    // Getters
    // -----------------------------------------------------------------------
    point3D get_min() const { return minimum; }
    point3D get_max() const { return maximum; };
    point3D get_centroid() const { return centroid; }

    // Functions
    // -----------------------------------------------------------------------
    /// Reference: Fundamentals of Computer Graphics - Section 12.3.1: Bounding Boxes
    bool intersection(const Ray& r, double t_min, double t_max) const {
        // Checks if ray r intersect the AABB within the interval [t_min,t_max]
        // NOTE: When measuring runtime, don't call this function; instead, paste the intersection code here.

       //  num_calls_box_intersection++;            // how many times is this function called?

       return Williams_ray_AABB_intersection(r, t_min, t_max);
       // return Tavian_ray_AABB_intersection(r, t_min, t_max);
       // return slab_method(r, t_min, t_max);
       // return our_ray_AABB_intersection(r, t_min, t_max);
       // return Kensler_ray_AABB_intersection_method(r, t_min, t_max);
    }

    inline double volume() const {
        // Calculates the volume of the AABB
        return (maximum.x() - minimum.x()) * (maximum.y() - minimum.y()) * (maximum.z() - minimum.z());
    }

private:
    bool slab_method(const Ray& r, double t_min, double t_max) const {
        Vec3D r_orig = r.get_ray_origin();
        Vec3D r_direction = r.get_ray_direction();

        for (int a = 0; a < 3; a++) {
            auto t0 = fmin((minimum[a] - r_orig[a]) / r_direction[a],
                           (maximum[a] - r_orig[a]) / r_direction[a]);
            auto t1 = fmax((minimum[a] - r_orig[a]) / r_direction[a],
                           (maximum[a] - r_orig[a]) / r_direction[a]);
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    /// Reference: Fast, Branchless Ray/Bounding Box Intersections
    bool Tavian_ray_AABB_intersection(const Ray& r, double t_min, double t_max) const {
        /// This is an implementation of the fast ray/box intersection algorithm.
        /// It is often referred to as "branchless". There are some good details about
        /// it in Tavian's blog: https://tavianator.com/2011/ray_box.html and Peter
        /// Shirley's blog: https://psgraphics.blogspot.com/2016/02/ray-box-intersection-and-fmin.html

        // Cache - No need to repeatedly call the function.
        Vec3D ray_origin = r.get_ray_origin();

        // We need the inverse of the ray's direction. Also, I don't want to divide by the
        // inverse, I will instead multiply be the reciprocal.
        for (int a = 0; a < 3; a++) {
            auto t0 = std::min((minimum[a] - ray_origin[a]) * r.inv_direction[a],
                           (maximum[a] - ray_origin[a]) * r.inv_direction[a]);
            auto t1 = std::max((minimum[a] - ray_origin[a]) * r.inv_direction[a],
                           (maximum[a] - ray_origin[a]) * r.inv_direction[a]);
            t_min = std::max(t0, t_min);
            t_max = std::min(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return t_min < t_max;
    }

    bool our_ray_AABB_intersection(const Ray& r, double t_min, double t_max) const {
        Vec3D ray_origin = r.get_ray_origin();

        for (int i = 0; i < 3; ++i) {
            double inv_direction_i = r.inv_direction[i];
            double t0 = (minimum[i] - ray_origin[i]) * inv_direction_i;
            double t1 = (maximum[i] - ray_origin[i]) * inv_direction_i;

            if (t0 > t1)
                std::swap(t0, t1);

            t_min = std::max(t0, t_min);
            t_max = std::min(t1, t_max);

            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    /// Reference: https://psgraphics.blogspot.com/2016/02/ray-box-intersection-and-fmin.html
    bool Kensler_ray_AABB_intersection_method(const Ray& r, double t_min, double t_max) const {
        Vec3D r_orig = r.get_ray_origin();

        for (int a = 0; a < 3; a++) {
            auto t0 = (minimum[a] - r_orig[a]) * r.inv_direction[a];
            auto t1 = (maximum[a] -  r_orig[a]) * r.inv_direction[a];
            if (r.inv_direction[a] < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    /// Reference: An Efficient and Robust Ray–Box Intersection Algorithm - https://people.csail.mit.edu/amy/papers/box-jgt.pdf
    bool Williams_ray_AABB_intersection(const Ray& r, double t_min, double t_max) const {
        double tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (bounds[r.sign[0]].x() - r.get_ray_origin().x()) * r.inv_direction.x();
        tmax = (bounds[1 - r.sign[0]].x() - r.get_ray_origin().x()) * r.inv_direction.x();
        tymin = (bounds[r.sign[1]].y() - r.get_ray_origin().y()) * r.inv_direction.y();
        tymax = (bounds[1 - r.sign[1]].y() - r.get_ray_origin().y()) * r.inv_direction.y();

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bounds[r.sign[2]].z() - r.get_ray_origin().z()) * r.inv_direction.z();
        tzmax = (bounds[1 - r.sign[2]].z() - r.get_ray_origin().z()) * r.inv_direction.z();

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return ((tmin < t_max) && (tmax > t_min));
    }

    // Data Members
    // -----------------------------------------------------------------------
    point3D minimum;            // point at the minimum corner of the box
    point3D maximum;            // point at the maximum corner of the box
    Vec3D bounds[2];            // the two corner of the box
    point3D centroid;
};

// Supporting Functions
// -----------------------------------------------------------------------
/// Reference: Reference: Fundamentals of Computer Graphics - Section 12.3.1: Bounding Boxes
AABB construct_surrounding_box(AABB box_0, AABB box_1) {
    // Constructs a box surrounding two other boxes: box_0 and box_1

    point3D minimum(fmin(box_0.get_min().x(), box_1.get_min().x()),
                    fmin(box_0.get_min().y(), box_1.get_min().y()),
                    fmin(box_0.get_min().z(), box_1.get_min().z()));

    point3D maximum(fmax(box_0.get_max().x(), box_1.get_max().x()),
                    fmax(box_0.get_max().y(), box_1.get_max().y()),
                    fmax(box_0.get_max().z(), box_1.get_max().z()));

    return {minimum, maximum};
}

#endif //CUDA_RAY_TRACER_AABB_H
