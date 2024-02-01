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

        // First, check the intersection along the x-axis
        auto inv_dir = Vec3D(1.0 / r.get_ray_direction().x(), 1.0 / r.get_ray_direction().y(), 1.0 / r.get_ray_direction().z());
        auto t_x_min = (minimum[0] - r.get_ray_origin()[0]) * inv_dir[0];
        auto t_x_max = (maximum[0] - r.get_ray_origin()[0]) * inv_dir[0];
        t_min = fmax(fmin(t_x_min, t_x_max), t_min);
        t_max = fmin(fmax(t_x_min, t_x_max), t_max);
        if (t_max <= t_min)
            return false;

        // Then, check the intersection along the y-axis
        auto t_y_min = (minimum[1] - r.get_ray_origin()[1]) * inv_dir[1];
        auto t_y_max = (maximum[1] - r.get_ray_origin()[1]) * inv_dir[1];
        t_min = fmax(fmin(t_y_min, t_y_max), t_min);
        t_max = fmin(fmax(t_y_min, t_y_max), t_max);
        if (t_max <= t_min)
            return false;

        // Finally, check the intersection along the z-axis
        auto t_z_min = (minimum[2] - r.get_ray_origin()[2]) * inv_dir[2];
        auto t_z_max = (maximum[2] - r.get_ray_origin()[2]) * inv_dir[2];
        t_min = fmax(fmin(t_z_min, t_z_max), t_min);
        t_max = fmin(fmax(t_z_min, t_z_max), t_max);
        if (t_max <= t_min)
            return false;

        return true;

        /* /// Reference: Ray Tracing: The Next Week
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.get_ray_direction()[a];
            auto t0 = (get_min()[a] - r.get_ray_origin()[a]) * invD;
            auto t1 = (get_max()[a] - r.get_ray_origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;*/
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
