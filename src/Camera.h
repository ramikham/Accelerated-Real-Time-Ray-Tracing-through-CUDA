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
    Camera();
    Camera(
            const point3D& camera_origin,
            double aspect_ratio,
            double focal_length,
            int image_width,
            double viewport_height
           ) :
    camera_origin(camera_origin),
    aspect_ratio(aspect_ratio),
    focal_length(focal_length),
    image_width(image_width),
    viewport_height(viewport_height) {
        image_height = static_cast<int>(image_width/aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
        viewport_u = Vec3D(viewport_width, 0, 0);
        viewport_v = Vec3D(0, -viewport_height, 0);
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        viewport_upper_left_corner = camera_origin - Vec3D(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        first_pixel_center_location = viewport_upper_left_corner + 0.5 * (pixel_delta_u + pixel_delta_v);
    }


public:
    // Parameters
    point3D camera_origin;
    double aspect_ratio;
    double focal_length;
    int image_width;
    double viewport_height;

    // Dependent
    int image_height;
    double viewport_width;
    Vec3D viewport_u;
    Vec3D viewport_v;
    Vec3D pixel_delta_u;
    Vec3D pixel_delta_v;
    Vec3D viewport_upper_left_corner;
    Vec3D first_pixel_center_location;
};
#endif //CUDA_RAY_TRACER_CAMERA_H
