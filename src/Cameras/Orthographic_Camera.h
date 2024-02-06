//
// Created by Rami on 2/6/2024.
//

#ifndef CUDA_RAY_TRACER_ORTHOGRAPHIC_CAMERA_H
#define CUDA_RAY_TRACER_ORTHOGRAPHIC_CAMERA_H

#include "../Utilities.h"

/// Reference: Fundamentals of Computer Graphics - Section 4.3.1: Orthographic Views
class Orthographic_Camera {
public:
    Orthographic_Camera( point3D lookfrom,           // the position where we place the camera (the camera's origin - now settable by the user)
                         point3D lookat,             // the point we look at
                         Vec3D vup,                  // a vector representing the "up" direction of the camera
                         double vfov,                // vertical field of view - in degrees
                         double aspect_ratio         // the proportional relationship between the width and the height of the display
    ) {
        camera_origin = lookfrom;

        double theta = degrees_to_radians(vfov);
        double h = tan(theta/2);
        image_height = 2.0 * h;
        image_width = aspect_ratio * image_height;

        l = -r;
        b = -t;
        r = image_width;
        t = image_height;

        // Compute the camera's orthonormal basis, its coordiante system
        camera_backward = unit_vector(lookfrom - lookat);
        camera_right = unit_vector(cross_product(vup, camera_backward));
        camera_up = cross_product(camera_backward, camera_right);
    }

    Ray generate_orthographic_viewing_rays(double i, double j) const{
        // Compute u and v using their respective equation (Pg. )
        std::pair<double,double> u_and_v = get_coord_on_image_plane(i,j);
        double u = u_and_v.first;
        double v = u_and_v.second;

        // Compute the ray direction
        Vec3D ray_direction = -camera_backward;

        // Compute the ray's origin
        Vec3D ray_origin = camera_origin + u * camera_right + v * camera_up;

        return Ray(ray_direction, ray_origin);
    }

private:
    // Supporting Functions
    std::pair<double,double> get_coord_on_image_plane(double i, double j) const {
        // (i,j): pixel in the raster image plane
        // returns a ray sent from the camera;s origin towards (u,v).

        double u = l + (r - l) * (i + 0.5) / image_width;
        double v = b + (t - b) * (j + 0.5) / image_height;

        return std::make_pair(u,v);
    }

    // Data Members

    double l;       // left edge of the image
    double r;       // right edge of the image
    double b;       // bottom of the image plane
    double t;       // top of the image plane

    double image_width;
    double image_height;

    Vec3D camera_right, camera_up, camera_backward;         // vectors that define the camera's plane

    point3D camera_origin;          // the point at which the camera (eye) looks from

// Note (from reference): "We can assume that l = -r and b = -t so that
// width and height suffice". I adopt this in my code, but leave l and b
// to be explicit for future changes if wanted.
};
#endif //CUDA_RAY_TRACER_ORTHOGRAPHIC_CAMERA_H
