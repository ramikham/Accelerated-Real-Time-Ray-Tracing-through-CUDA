//
// Created by Rami on 2/8/2024.
//

#ifndef CUDA_RAY_TRACER_PARALLEL_RENDERING_FUNCTIONS_H
#define CUDA_RAY_TRACER_PARALLEL_RENDERING_FUNCTIONS_H

#include "../Utilities.h"
#include "../Cameras/Camera.h"
#include "../Cameras/Orthographic_Camera.h"
#include "../Shading.h"
#include "../Scenes.h"


// Functions that render with the radiance(...) function
// -----------------------------------------------------------------------
void parallel_loop_radiance_renderer(Scene_Information& scene_info) {
    // Using OpenMP to parallelize for loops require that the later be in Canonical form. This means I
    // can't write "i < scene_info.image_height" and instead need to write "i < image_height" in the
    // render loop. For this reason, I create variables to store all the data members of the struct
    // Scene_Information. It would be easier to skip Scene_Information all together and just assign the
    // variables inside this function, but my goal in writing Scene_Information is that every scene may
    // have a different camera/image/render settings, and including all possible choices will make this
    // function long.

    /* Parallelization Strategy: Uses the collapse(2) clause to parallelize the render loop */

    // Get scene information
    // -------------------------------------------------------------------------------

    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = scene_info.aspect_ratio;
    const int image_width = scene_info.image_width;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    int max_depth = scene_info.max_depth;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = scene_info.world;
    Primitives_Group lights = scene_info.lights;
    int samples_per_pixel = scene_info.samples_per_pixel;

    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom = scene_info.lookfrom;
    point3D lookat = scene_info.lookat;
    Vec3D vup = scene_info.vup;
    double vfov  = scene_info.vfov;
    Camera cam = scene_info.camera;

    const std::string& file_name = scene_info.output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;

    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

#pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, cam, world, pixel_colors, max_depth, lights) num_threads(16)
    for (int j = image_height - 1; j >=0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
            for (int s = 0; s < samples_per_pixel; ++s) {
                //   std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = cam.get_ray(u, v);

                // Accumulate color for each sample
                pixel_color += radiance(r, world, max_depth);
            }

            pixel_colors[j][i] = pixel_color;
        }
    }

    std::cerr << "\nDone.\n";

    // Average the colors from all samples to compute the final pixel color
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color = pixel_colors[j][i];

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Apply 2-gamma
            double r_comp = pixel_color.x();
            double g_comp = pixel_color.y();
            double b_comp = pixel_color.z();

            r_comp = gamma_2_correction(r_comp);
            g_comp = gamma_2_correction(g_comp);
            b_comp = gamma_2_correction(b_comp);

            // Write the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// TODO: Write other parallel strategies

// Functions that render with the radiance_background(...) function
// -----------------------------------------------------------------------
void parallel_loop_radiance_background_renderer(Scene_Information& scene_info) {
    /* Parallelization Strategy: Uses the collapse(2) clause to parallelize the render loop */

    // Get scene information
    // -------------------------------------------------------------------------------

    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = scene_info.aspect_ratio;
    const int image_width = scene_info.image_width;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    int max_depth = scene_info.max_depth;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = scene_info.world;
    Primitives_Group lights = scene_info.lights;
    int samples_per_pixel = scene_info.samples_per_pixel;

    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom = scene_info.lookfrom;
    point3D lookat = scene_info.lookat;
    Vec3D vup = scene_info.vup;
    double vfov  = scene_info.vfov;
    Camera cam = scene_info.camera;

    const std::string& file_name = scene_info.output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;

    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

#pragma omp parallel for collapse(2) default(none) shared(std::cout, samples_per_pixel, image_height, image_width, cam, world, pixel_colors, max_depth, lights)
    for (int j = image_height - 1; j >=0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
            for (int s = 0; s < samples_per_pixel; ++s) {
                //  std::cout << "Number of active threads = " << omp_get_num_threads() << std::endl;
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = cam.get_ray(u, v);

                // Accumulate color for each sample
                pixel_color += radiance_background(r, world, max_depth);
            }

            pixel_colors[j][i] = pixel_color;
        }
    }

    std::cerr << "\nDone.\n";

    // Average the colors from all samples to compute the final pixel color
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color = pixel_colors[j][i];

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Apply 2-gamma
            double r_comp = pixel_color.x();
            double g_comp = pixel_color.y();
            double b_comp = pixel_color.z();

            r_comp = gamma_2_correction(r_comp);
            g_comp = gamma_2_correction(g_comp);
            b_comp = gamma_2_correction(b_comp);

            // Write the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// TODO: Write other parallel strategies

void parallel_cols_workload_radiance_background(Scene_Information& scene_info) {
    /* Parallelization Strategy: Distribute the workload based on columns */

    // Get scene information
    // -------------------------------------------------------------------------------

    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = scene_info.aspect_ratio;
    const int image_width = scene_info.image_width;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    int max_depth = scene_info.max_depth;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = scene_info.world;
    Primitives_Group lights = scene_info.lights;
    int samples_per_pixel = scene_info.samples_per_pixel;

    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom = scene_info.lookfrom;
    point3D lookat = scene_info.lookat;
    Vec3D vup = scene_info.vup;
    double vfov  = scene_info.vfov;
    Camera cam = scene_info.camera;

    const std::string& file_name = scene_info.output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;

    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";

    std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

    // Determine the number of available threads
    int num_threads = omp_get_max_threads();

    // Determine the number of columns each thread will handle
    int cols_per_thread = image_width / num_threads;

// Render Loop with parallelization based on columns
#pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        int start_col = thread_id * cols_per_thread;
        int end_col = (thread_id == num_threads - 1) ? image_width : start_col + cols_per_thread;

        // Loop over the assigned columns
        for (int i = start_col; i < end_col; ++i) {
            for (int j = image_height - 1; j >= 0; --j) {
                Color pixel_color(0.0, 0.0, 0.0);

                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);

                    Ray r = cam.get_ray(u, v);
                    pixel_color += radiance_background(r, world, max_depth);
                }

                pixel_colors[j][i] = pixel_color;
            }
        }
    }

    std::cerr << "\nDone.\n";
    // Average the colors from all samples to compute the final pixel color

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color = pixel_colors[j][i];

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Apply 2-gamma
            double r_comp = pixel_color.x();
            double g_comp = pixel_color.y();
            double b_comp = pixel_color.z();

            r_comp = gamma_2_correction(r_comp);
            g_comp = gamma_2_correction(g_comp);
            b_comp = gamma_2_correction(b_comp);

            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// TODO: Write parallel_rows_workload_radiance(...)

void parallel_tasks_radiance_background_renderer(Scene_Information& scene_info) {
    /* Parallelization Strategy: task parallelism */

    // Get scene information
    // -------------------------------------------------------------------------------

    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = scene_info.aspect_ratio;
    const int image_width = scene_info.image_width;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    int max_depth = scene_info.max_depth;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = scene_info.world;
    Primitives_Group lights = scene_info.lights;
    int samples_per_pixel = scene_info.samples_per_pixel;

    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom = scene_info.lookfrom;
    point3D lookat = scene_info.lookat;
    Vec3D vup = scene_info.vup;
    double vfov  = scene_info.vfov;
    Camera cam = scene_info.camera;

    const std::string& file_name = scene_info.output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;

    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

    // Get the number of available threads
    int num_threads = omp_get_max_threads();

    // Task Parallelism
#pragma omp parallel num_threads(num_threads)
    {
#pragma omp for schedule(dynamic)
        for (int task = 0; task < image_width * image_height; ++task) {
            int i = task % image_width;
            int j = task / image_width;

            Color pixel_color(0.0, 0.0, 0.0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                Ray r = cam.get_ray(u, v);
                pixel_color += radiance_background(r, world, max_depth);
            }

            pixel_colors[j][i] = pixel_color;
        }
    }

    std::cerr << "\nDone.\n";

    // Fill the image file with the correct order of pixels
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color = pixel_colors[j][i];

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Apply 2-gamma
            double r_comp = pixel_color.x();
            double g_comp = pixel_color.y();
            double b_comp = pixel_color.z();

            r_comp = gamma_2_correction(r_comp);
            g_comp = gamma_2_correction(g_comp);
            b_comp = gamma_2_correction(b_comp);

            // Write the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// Functions that render with the radiance_mixture(...) function
// -----------------------------------------------------------------------
void parallel_loop_radiance_mixture_renderer(Scene_Information& scene_info) {
    /* Parallelization Strategy: Uses the collapse(2) clause to parallelize the render loop */

    // Get scene information
    // -------------------------------------------------------------------------------

    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = scene_info.aspect_ratio;
    const int image_width = scene_info.image_width;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    int max_depth = scene_info.max_depth;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = scene_info.world;
    Primitives_Group lights = scene_info.lights;
    int samples_per_pixel = scene_info.samples_per_pixel;

    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom = scene_info.lookfrom;
    point3D lookat = scene_info.lookat;
    Vec3D vup = scene_info.vup;
    double vfov  = scene_info.vfov;
    Camera cam = scene_info.camera;

    const std::string& file_name = scene_info.output_image_name + ".ppm";
    std::ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);

    std::cout << "Image height = " << image_height << std::endl;
    std::cout << "Image Width = " << image_width << std::endl;
    std::cout << "Depth = " << max_depth << std::endl;
    std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;

    // Render Loop
    // -----------------------------------------------------------------------
    // Reference: How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
    ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

#pragma omp parallel for collapse(2) schedule(static) default(none) shared(samples_per_pixel, image_height, image_width, cam, world, pixel_colors, max_depth, lights)
    for (int j = image_height - 1; j >=0; --j) {
        //    #pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, j, cam, world, lights, pixel_colors, max_depth) num_threads(16)
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
            //   #pragma omp parallel for schedule(dynamic) default(none) shared(samples_per_pixel, i, j, cam, pixel_color, world, lights, image_width, image_height, max_depth) num_threads(16)
            for (int s = 0; s < samples_per_pixel; ++s) {
                //   std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = cam.get_ray(u, v);

                // Accumulate color for each sample
                pixel_color += radiance_mixture(r, world, lights, max_depth);
            }

            pixel_colors[j][i] = pixel_color;
        }
    }

    std::cerr << "\nDone.\n";
    // Average the colors from all samples to compute the final pixel color
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color = pixel_colors[j][i];

            // Average the colors from all samples
            pixel_color /= samples_per_pixel;

            // Apply 2-gamma
            double r_comp = pixel_color.x();
            double g_comp = pixel_color.y();
            double b_comp = pixel_color.z();

            r_comp = gamma_2_correction(r_comp);
            g_comp = gamma_2_correction(g_comp);
            b_comp = gamma_2_correction(b_comp);

            // Write the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// TODO: Write other parallel strategies
#endif //CUDA_RAY_TRACER_PARALLEL_RENDERING_FUNCTIONS_H