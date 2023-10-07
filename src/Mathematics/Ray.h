//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_RAY_H
#define CUDA_RAY_TRACER_RAY_H

#include "../Utilities.h"

/// A class to represent a ray. A ray is given is a function R(t) = origin + t * direction
class Ray {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Ray() {}
    Ray(const point3D& ray_origin, const Vec3D& ray_direction, double time=0.0)
    : ray_origin(ray_origin), ray_direction(ray_direction), time(time) {}

    // Getters
    // -----------------------------------------------------------------------
    point3D get_ray_origin() const;
    Vec3D get_ray_direction() const;
    double get_time() const;
    point3D at(double t) const;

private:
    point3D ray_origin;
    point3D ray_direction;
    double time;
};


#endif //CUDA_RAY_TRACER_RAY_H
