//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_CAMERA_H
#define CUDA_RAY_TRACER_CAMERA_H

#include "../Utilities.h"

/// Reference: Ray Tracing in One Weekend
class Camera {
public:
    // Constructors
    // -----------------------------------------------------------------------
    Camera() {}
    // Third constructor - positionable and orientable camera.
    Camera(
            point3D lookfrom,           // the position where we place the camera (the camera's origin - now settable by the user)
            point3D lookat,             // the point we look at
            Vec3D vup,                  // a vector representing the "up" direction of the camera
            double vfov,                // vertical field of view - in degrees
            double aspect_ratio         // the proportional relationship between the width and the height of the display
    ) {
        double theta = degrees_to_radians(vfov);                    // vfov angle
        auto h = tan(theta/2);                                   // half-height of the viewport
        auto viewport_height = 2.0 * h;                             // viewport height
        auto viewport_width = aspect_ratio * viewport_height;       // viewport width

        Vec3D camera_right, camera_up, camera_backward;                     // vectors that define the camera's plane
        camera_backward = unit_vector(lookfrom - lookat);
        camera_right = unit_vector(cross_product(vup, camera_backward));
        camera_up = cross_product(camera_backward, camera_right);

        camera_origin = lookfrom;
        viewport_horizontal_vector = viewport_width * camera_right;
        viewport_vertical_vector = viewport_height * camera_up;
        lower_left_corner = camera_origin - viewport_horizontal_vector/2 - viewport_vertical_vector/2 - camera_backward;
    }

    Ray get_ray(double u, double v) const{
        // (u,v): pixel in the image plane.
        // returns a ray sent from the camera;s origin towards (u,v).
        return Ray(camera_origin, lower_left_corner + u*viewport_horizontal_vector + v*viewport_vertical_vector - camera_origin);
    }


public:
    // Data Members
    // -----------------------------------------------------------------------
    point3D camera_origin;                          // the point at which the camera (eye) looks from
    point3D lower_left_corner;                      // the lower left corner of the viewport
    Vec3D viewport_horizontal_vector;               // horizontal width of the viewport
    Vec3D viewport_vertical_vector;                 // vertical width of the viewport
};
#endif //CUDA_RAY_TRACER_CAMERA_H
