//
// Created by Rami on 2/8/2024.
//

#ifndef CUDA_RAY_TRACER_SERIAL_RENDERING_FUNCTIONS_H
#define CUDA_RAY_TRACER_SERIAL_RENDERING_FUNCTIONS_H

#include "../Utilities.h"
#include "../Cameras/Camera.h"
#include "../Cameras/Orthographic_Camera.h"
#include "../Shading.h"
#include "../Scenes.h"


void serial_radiance_renderer(Scene_Information& scene_info) {
    // Output File
    // -------------------------------------------------------------------------------
    const std::string& file_name = scene_info.output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);


    // Render Loop
    // -----------------------------------------------------------------------

    // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << scene_info.image_width << " " << scene_info.image_height << "\n255\n";
    for (int j = scene_info.image_height - 1; j >=0; --j) {
        for (int i = 0; i < scene_info.image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);
            for (int s = 0; s < scene_info.samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (scene_info.image_width - 1);
                auto v = (j + random_double()) / (scene_info.image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = scene_info.camera.get_ray(u, v);

                // Accumulate color for each sample
                pixel_color += radiance(r, scene_info.world, scene_info.max_depth);
            }
            // Average color over all samples
            pixel_color /= scene_info.samples_per_pixel;

            // Write the color to the output file
            int ir = static_cast<int>(256 * clamp(pixel_color.x(), 0.0, 0.999));
            int ig = static_cast<int>(256 * clamp(pixel_color.y(), 0.0, 0.999));
            int ib = static_cast<int>(256 * clamp(pixel_color.z(), 0.0, 0.999));
            ofs << ir << " " << ig << " " << ib << "\n";
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

#endif //CUDA_RAY_TRACER_SERIAL_RENDERING_FUNCTIONS_H
