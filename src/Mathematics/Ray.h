//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_RAY_H
#define CUDA_RAY_TRACER_RAY_H

#include "../Utilities.h"

// A class to represent a ray. A ray is given is a function R(t) = origin + t * direction.
class Ray {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Ray() {}

    Ray(const point3D& ray_origin, const Vec3D& ray_direction, double time=0.0)
    : ray_origin(ray_origin), ray_direction(ray_direction), time(time) {
        inv_direction = Vec3D(1/ray_direction.x(), 1/ray_direction.y(), 1/ray_direction.z());
        sign[0] = (inv_direction.x() < 0);
        sign[1] = (inv_direction.y() < 0);
        sign[2] = (inv_direction.z() < 0);

    }

    // Getters
    // -----------------------------------------------------------------------
    point3D get_ray_origin() const {
        // Returns the origin point of the ray

        return ray_origin;
    };

    Vec3D get_ray_direction() const {
        // Returns the direction vector of the ray

        return ray_direction;
    };

    double get_time() const {
        // Returns the time of the ray

        return time;
    };

    point3D at(double t) const {
        // Solves the ray's equation at time = t

        return ray_origin + t * ray_direction - ray_direction * 0.0001;
    };

    Vec3D get_inverse_direction() const {
        // Returns the inverse of the direction vector of the ray

        return {1.0 / ray_direction.x(), 1.0 / ray_direction.y(), 1.0 / ray_direction.z()};
    };

public:
    // Data Members
    // -----------------------------------------------------------------------
    point3D ray_origin;             // the ray's origin
    Vec3D ray_direction;            // the ray's direction
    double time;                    // the ray's time
    Vec3D inv_direction;
    int sign[3];
};


#endif //CUDA_RAY_TRACER_RAY_H
