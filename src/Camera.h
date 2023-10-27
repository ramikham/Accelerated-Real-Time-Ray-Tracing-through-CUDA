//
// Created by Rami on 9/20/2023.
//

#ifndef CUDA_RAY_TRACER_CAMERA_H
#define CUDA_RAY_TRACER_CAMERA_H

#include "Utilities.h"

class Camera {
public:
    // Constructors
    // -----------------------------------------------------------------------
    /// Default Constructor
    /*
    Camera() {
        double aspect_ratio = 16.0 / 9.0;
        double viewport_height = 2.0;
        double viewport_width = aspect_ratio * viewport_height;
        focal_length = 1.0;

        camera_origin = point3D(0,0,0);
        viewport_width_vector = Vec3D(viewport_width, 0.0, 0.0);
        viewport_height_vector = Vec3D(0.0, viewport_height, 0.0);
        lower_left_corner = camera_origin -
                viewport_width_vector/2 -
                viewport_height_vector/2 -
                Vec3D(0, 0, focal_length);
    }

    /// Second Constructor: Enables a positionable camera by means of a pinhole camera model.
    /// Allows for adjustable vertical FoV.
    Camera (
            double v_fov,
            double aspect_ratio
            ) {
        double theta = degrees_to_radians(v_fov);
        double h = tan(theta / 2);
        double viewport_height = 2.0 * h;
        double viewport_width = aspect_ratio * viewport_height;

        focal_length = 1.0;

        camera_origin = point3D(0,0,0);
        viewport_width_vector = Vec3D(viewport_width, 0.0, 0.0);
        viewport_height_vector = Vec3D(0.0, viewport_height, 0.0);
        lower_left_corner = camera_origin -
                            viewport_width_vector/2 -
                            viewport_height_vector/2 -
                            Vec3D(0, 0, focal_length);
    }
*/
    // Third constructor: allows for camera orientation
    Camera(
            point3D lookfrom,           // the position where we place the camera (the camera's origin - now settable by the user)
            point3D lookat,             // the point we look at
            point3D vup,                // a vector representing the "up" direction of the camera
            double vfov,                // vertical field of view - in degrees
            double aspect_ratio
            ) {
        // Camera's Orthonormal Basis
        Vec3D camera_backward, camera_right, camera_up;

        double theta = degrees_to_radians(vfov);
        double half_viewport_height = tan(theta / 2);
        double half_viewport_width = aspect_ratio * half_viewport_height;
        camera_origin = lookfrom;

        // Setup the camera's orthonormal basis
        camera_backward = unit_vector(lookfrom - lookat);
        camera_right = unit_vector(cross_product(vup, camera_backward));
        camera_up = cross_product(camera_backward, camera_right);

        lower_left_corner = camera_origin
                            - camera_backward
                            - half_viewport_width * camera_right
                            - half_viewport_height * camera_up;

        viewport_width_vector = 2 * half_viewport_width * camera_right;
        viewport_height_vector = 2 * half_viewport_height * camera_up;

    }


    // Use this when using the first and second constructors
    Ray get_ray(double u, double v) const{
        /// Input: (u,v) is a point (i.e., pixel) in the image plane that the camera is capturing.
        /// u and v ∈ [0,1].
        /// Output: the method constructs and returns a ray, which starts at the camera's origin and goes through the
        /// point of the image plane represented by the u-scaled horizontal vector and the v-scaled vertical vector
        /// to the lower_left_corner, then subtracting the camera's origin
        return Ray(camera_origin, lower_left_corner + u*viewport_width_vector + v*viewport_height_vector - camera_origin);
    }


public:
    // Data Members
    // -----------------------------------------------------------------------
    point3D camera_origin;
    Vec3D viewport_width_vector;                    // (horizontal) a vector representing the width of the viewport in the camera's coordinate system.
                                                    // Points from the camera's origin to the right side of the viewport.


    Vec3D viewport_height_vector;                   // (vertical) a vector representing the height of the viewport in the camera's coordinate system.
                                                    // Points from the camera's origin to the top side of the viewport.

    point3D lower_left_corner;                      // the lower left corner of the viewport
};
#endif //CUDA_RAY_TRACER_CAMERA_H
