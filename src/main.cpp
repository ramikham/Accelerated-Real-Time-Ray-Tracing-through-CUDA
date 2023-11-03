#include "Utilities.h"
#include "Camera.h"
#include "Shading.h"
#include "Scenes.h"

/*

void rendering_loop(Camera cam, const std::string& output_image_name) {
    // Output File
    // -------------------------------------------------------------------------------
    const std::string& file_name = output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);


    // Render Loop
    // -----------------------------------------------------------------------

    // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << cam.image_width << " " << cam.image_height << "\n255\n";
    for (int j = 0; j < cam.image_height; ++j) {
        for (int i = 0; i < cam.image_width; ++i) {
            // Get the center of the pixel
            point3D pixel_center = cam.first_pixel_center_location + (i * cam.pixel_delta_u) + (j * cam.pixel_delta_v);

            // Get the ray direction from the camera origin to the pixel center
            point3D ray_direction = pixel_center - cam.camera_origin;

            // Construct a ray from the camera origin in the direction of the pixel center
            Ray r(cam.camera_origin, ray_direction);

            // Shoot a ray through the pixel center and color it
            Color pixel_color = shade(r, specular_scene());

            // Output the color the PPM file
            ofs << static_cast<int>(255.9 * pixel_color.x()) << ' '
                << static_cast<int>(255.9 * pixel_color.y()) << ' '
                << static_cast<int>(255.9 * pixel_color.z()) << '\n';
        }
    }
}

/*
void rendering_loop_with_supersampling(int samples_per_pixel, Camera cam, const std::string& output_image_name) {
    // Output File
    // -------------------------------------------------------------------------------
    const std::string& file_name = output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);


    // Render Loop
    // -----------------------------------------------------------------------

    // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << cam.image_width << " " << cam.image_height << "\n255\n";
    for (int j = 0; j < cam.image_height; ++j) {
        for (int i = 0; i < cam.image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
            // Multisampling loop
#pragma omp parallel for schedule(dynamic) default(none) shared(samples_per_pixel, i, j, cam, pixel_color) num_threads(16)
            for (int s = 0; s < samples_per_pixel; ++s) {
                // Calculate random offsets within the pixel
                double u_offset = (i+random_double()) / (cam.image_width - 1);
                double v_offset = (j+random_double()) / (cam.image_height - 1);

                // Calculate the ray direction from the camera origin to the sample point
                point3D ray_direction = cam.viewport_upper_left_corner + u_offset * cam.viewport_u + v_offset * cam.viewport_v - cam.camera_origin;

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r(cam.camera_origin, ray_direction);

                // Accumulate color for each sample
                pixel_color += shade(r, many_objects_scene(), 10);
            }

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(pixel_color.x(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.y(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.z(), 0.0, 0.999)) << '\n';
        }
    }
}
*/
void rendering_loop(int samples_per_pixel, const std::string& output_image_name) {
    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 5;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = many_objects_scene();


    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom(13,2,3);       //point3D lookfrom(-2, 2, 1);
    point3D lookat(0, 0, 0);        //point3D lookat(0,0,-1);
    Vec3D vup(0,1,0);
    auto vfov = 20.0;

    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

    // Output File
    // -------------------------------------------------------------------------------
    const std::string& file_name = output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;
    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >=0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
           //
           // #pragma omp parallel for schedule(dynamic) default(none) shared(std::cout, samples_per_pixel, i, j, cam, pixel_color, world, image_width, image_height) num_threads(16)
            for (int s = 0; s < samples_per_pixel; ++s) {
                //std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = cam.get_ray(u, v);

                // Accumulate color for each sample
                pixel_color += shade(r, world, max_depth);
            }

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Output the averaged color to the PPM file
           ofs << static_cast<int>(255 * clamp(pixel_color.x(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.y(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.z(), 0.0, 0.999)) << '\n';
        }
    }
}

void parallel_rendering_loop(int samples_per_pixel, const std::string& output_image_name) {
    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 5;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = many_objects_scene();


    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom(13,2,3);       //point3D lookfrom(-2, 2, 1);
    point3D lookat(0, 0, 0);        //point3D lookat(0,0,-1);
    Vec3D vup(0,1,0);
    auto vfov = 30.0;

    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

    // Output File
    // -------------------------------------------------------------------------------
    const std::string& file_name = output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;
    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

    for (int j = image_height - 1; j >=0; --j) {
        #pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, j, cam, world, pixel_colors) num_threads(16)
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
            // #pragma omp parallel for schedule(dynamic) default(none) shared(std::cout, samples_per_pixel, i, j, cam, pixel_color, world, image_width, image_height) num_threads(16)
            for (int s = 0; s < samples_per_pixel; ++s) {
                //std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = cam.get_ray(u, v);

                // Accumulate color for each sample
                pixel_color += shade(r, world, max_depth);
            }

            pixel_colors[j][i] = pixel_color;
        }
    }

    // Average the colors from all samples to compute the final pixel color
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color = pixel_colors[j][i];
            // Average the colors from all samples
            pixel_color /= samples_per_pixel;
            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(pixel_color.x(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.y(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.z(), 0.0, 0.999)) << '\n';
        }
    }
}
int main() {
    /* Reference:   The math for the image and camera details was inspired from:
     * https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
     */

    auto start = omp_get_wtime();
    //rendering_loop(50, "Testing Many Objects");
    parallel_rendering_loop(5, "Testing");
    auto stop = omp_get_wtime();
    auto duration = stop - start;

    std::cout << duration << std::endl;
    return 0;
}
