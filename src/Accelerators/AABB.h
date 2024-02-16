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
    AABB(const point3D& MIN, const point3D& MAX) : minimum(MIN), maximum(MAX) {}

    // Getters
    // -----------------------------------------------------------------------
    point3D get_min() const { return minimum; }
    point3D get_max() const { return maximum; };

    // Functions
    // -----------------------------------------------------------------------
    /// Reference: Fundamentals of Computer Graphics - Section 12.3.1: Bounding Boxes
    bool intersection(const Ray& r, double t_min, double t_max) const {
        /// Checks if ray r intersect the AABB within the interval [t_min,t_max]
        /// This is an implementation of the fast ray/box intersection algorithm.
        /// It is often referred to as "branchless". There are some good details about
        /// it in Tavian's blog: https://tavianator.com/2011/ray_box.html and Peter
        /// Shirley's blog: https://psgraphics.blogspot.com/2016/02/ray-box-intersection-and-fmin.html

        // We need the inverse of the ray's direction
        for (int a = 0; a < 3; a++) {
            auto t0 = fmin((minimum[a] - r.get_ray_origin()[a]) / r.get_ray_direction()[a],
                           (maximum[a] - r.get_ray_origin()[a]) / r.get_ray_direction()[a]);
            auto t1 = fmax((minimum[a] - r.get_ray_origin()[a]) / r.get_ray_direction()[a],
                           (maximum[a] - r.get_ray_origin()[a]) / r.get_ray_direction()[a]);
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

    AABB pad() {
        // Return an AABB that has no side narrower than some delta, padding if necessary.
        double delta = 0.0001;

        point3D new_min(
                (maximum.x() - minimum.x() >= delta) ? minimum.x() : (minimum.x() - delta / 2),
                (maximum.y() - minimum.y() >= delta) ? minimum.y() : (minimum.y() - delta / 2),
                (maximum.z() - minimum.z() >= delta) ? minimum.z() : (minimum.z() - delta / 2)
        );

        point3D new_max(
                (maximum.x() - minimum.x() >= delta) ? maximum.x() : (maximum.x() + delta / 2),
                (maximum.y() - minimum.y() >= delta) ? maximum.y() : (maximum.y() + delta / 2),
                (maximum.z() - minimum.z() >= delta) ? maximum.z() : (maximum.z() + delta / 2)
        );

        return AABB(new_min, new_max);
    }

private:
    // Data Members
    // -----------------------------------------------------------------------
    point3D minimum;            // point at the minimum extent (corner) of the box
    point3D maximum;            // point at the maximum extent (corner) of the box
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
