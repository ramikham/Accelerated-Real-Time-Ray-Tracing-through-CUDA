//
// Created by Rami on 10/17/2023.
//

#ifndef CUDA_RAY_TRACER_AXIS_ALIGNED_BOUNDING_BOX_H
#define CUDA_RAY_TRACER_AXIS_ALIGNED_BOUNDING_BOX_H

#include "../Utilities.h"

class Axis_Aligned_Bounding_Box {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Axis_Aligned_Bounding_Box() {}
    Axis_Aligned_Bounding_Box(const point3D& MIN, const point3D& MAX) : minimum(MIN), maximum(MAX) {}

    // Getters
    // -----------------------------------------------------------------------
    point3D get_min() const { return minimum; }
    point3D get_max() const { return maximum; };

    // Functions
    // -----------------------------------------------------------------------
    bool intersection(const Ray& r, double t_min, double t_max) const {
        /// Does ray r intersect the AABB within the interval [t_min,t_max]?

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
        }
private:
    // Data Members
    // -----------------------------------------------------------------------
    point3D minimum;
    point3D maximum;
};

Axis_Aligned_Bounding_Box construct_surrounding_box(Axis_Aligned_Bounding_Box box_0, Axis_Aligned_Bounding_Box box_1) {
    point3D minimum(fmin(box_0.get_min().x(), box_1.get_min().x()),
                    fmin(box_0.get_min().y(), box_1.get_min().y()),
                    fmin(box_0.get_min().z(), box_1.get_min().z()));

    point3D maximum(fmax(box_0.get_max().x(), box_1.get_max().x()),
                    fmax(box_0.get_max().y(), box_1.get_max().y()),
                    fmax(box_0.get_max().z(), box_1.get_max().z()));

    return {minimum, maximum};
}
#endif //CUDA_RAY_TRACER_AXIS_ALIGNED_BOUNDING_BOX_H
