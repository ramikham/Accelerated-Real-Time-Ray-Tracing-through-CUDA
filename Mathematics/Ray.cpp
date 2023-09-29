//
// Created by Rami on 9/20/2023.
//

#include "Ray.h"

point3D Ray::get_ray_origin() const {
    return ray_origin;
}

Vec3D Ray::get_ray_direction() const {
    return ray_direction;
}

double Ray::get_time() const {
    return time;
}

point3D Ray::at(double t) const {
    return ray_origin + t * ray_direction;
}
