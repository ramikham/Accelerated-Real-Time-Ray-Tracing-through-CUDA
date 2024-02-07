#include "Utilities.h"
#include "Cameras/Camera.h"
#include "Shading.h"
#include "Scenes.h"
#include "Cameras/Orthographic_Camera.h"

// Rendering with the radiance(...) function
void parallel_render_radiance(Scene_Information& scene_info) {
    // Using OpenMP to parallelize for loops require that the later be in Canonical form. This means I
    // can't write "i<scene_info.image_height" and instead need to write "i<image_height". For this
    // reason, I create variables to store all the data members of the struct Scene_Information. It
    // would be easier to skip Scene_Information all together and just assign the variables inside
    // this function, but my goal in writing Scene_Information is that every scene may have a different
    // camera/image/render settings, and including all possible choices will make this function long.


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

            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// Rendering with the radiance(...) function
void orthographic_view_renderer(Scene_Information& scene_info) {
    // Using OpenMP to parallelize for loops require that the later be in Canonical form. This means I
    // can't write "i<scene_info.image_height" and instead need to write "i<image_height". For this
    // reason, I create variables to store all the data members of the struct Scene_Information. It
    // would be easier to skip Scene_Information all together and just assign the variables inside
    // this function, but my goal in writing Scene_Information is that every scene may have a different
    // camera/image/render settings, and including all possible choices will make this function long.


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
    Orthographic_Camera  camera(lookfrom, lookat, vup, vfov, aspect_ratio);


    const std::string& file_name = scene_info.output_image_name + " - Orthographic Camera" + ".ppm";
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

    #pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, camera, world, pixel_colors, max_depth, lights) num_threads(16)
    for (int j = image_height - 1; j >=0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
            for (int s = 0; s < samples_per_pixel; ++s) {
                //   std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r = camera.generate_orthographic_viewing_rays(u, v);

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

            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name + " - Orthographic Camera" << std::endl;
}

// Rendering with the radiance_background(...) function
void parallel_render_radiance_background(Scene_Information& scene_info) {
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

    #pragma omp parallel for collapse(2) default(none) shared(std::cout, samples_per_pixel, image_height, image_width, cam, world, pixel_colors, max_depth, lights) num_threads(16)
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

            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

// Rendering with the radiance_background(...) function
void parallel_render_radiance_background_2(Scene_Information& scene_info) {
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

    // Determine the number of rows each thread will handle
    int rows_per_thread = image_height / num_threads;

    // Render Loop with parallelization based on rows
#pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        int start_row = thread_id * rows_per_thread;
        int end_row = (thread_id == num_threads - 1) ? image_height : start_row + rows_per_thread;

        // Loop over the assigned rows
        for (int j = end_row - 1; j >= start_row; --j) {
            for (int i = 0; i < image_width; ++i) {
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

// Rendering with the radiance_mixture(...) function
void parallel_render_radiance_mixture(Scene_Information& scene_info) {
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

            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
        }
    }

    std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

void render(Scene_Information& scene_info) {
    // Using OpenMP to parallelize for loops require that the later be in Canonical form. This means I
    // can't write "i<scene_info.image_height" and instead need to write "i<image_height". For this
    // reason, I create variables to store all the data members of the struct Scene_Information. It
    // would be easier to skip Scene_Information all together and just assign the variables inside
    // this function, but my goal in writing Scene_Information is that every scene may have a different
    // camera/image/render settings, and including all possible choices will make this function long.


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

     std::vector<Diffuse_Light> l;
     auto ob = Diffuse_Light(Color(20,20,20), 100, -10.0, 0.0, 80.0, 80.0, -10.0, 0.0);
     l.push_back(ob);

     std::cout << "Image height = " << image_height << std::endl;
     std::cout << "Image Width = " << image_width << std::endl;
     std::cout << "Depth = " << max_depth << std::endl;
     std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;
     // Render Loop
     // -----------------------------------------------------------------------
     // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
     ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
     std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

     #pragma omp parallel for default(none) schedule(dynamic) shared(samples_per_pixel, image_height, image_width, cam, world, l, pixel_colors, max_depth, lights) num_threads(16)
     for (int j = image_height - 1; j >=0; --j) {
         // std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
         #pragma omp parallel for default(none) schedule(dynamic) shared(samples_per_pixel, image_height, image_width, j, cam, world, l, lights, pixel_colors, max_depth) num_threads(16)
         for (int i = 0; i < image_width; ++i) {
             Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
             #pragma omp parallel for schedule(dynamic) default(none) shared(samples_per_pixel, i, j, cam, pixel_color, l, world, lights, image_width, image_height, max_depth) num_threads(16)
             for (int s = 0; s < samples_per_pixel; ++s) {
              //   std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                 auto u = (i + random_double()) / (image_width - 1);
                 auto v = (j + random_double()) / (image_height - 1);

                 // Construct a ray from the camera origin in the direction of the sample point
                 Ray r = cam.get_ray(u, v);

                 // Accumulate color for each sample
               //  pixel_color +=  radiance_background(r, world, max_depth);
                 pixel_color += radiance_sample_light_directly_2(r, world, l, max_depth);
             //  pixel_color += radiance_mixture(r, world, lights, max_depth);
              //   pixel_color += radiance_mixture(r, world,lights, max_depth);
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

             // Output the averaged color to the PPM file
             ofs << static_cast<int>(255 * clamp(r_comp, 0.0, 0.999)) << ' '
                 << static_cast<int>(255 * clamp(g_comp, 0.0, 0.999)) << ' '
                 << static_cast<int>(255 * clamp(b_comp, 0.0, 0.999)) << '\n';
         }
     }

     std::cout << "Name of file rendered: " << scene_info.output_image_name << std::endl;
}

/*
void serial_render(Scene_Information& scene_info) {
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
} */

bool correct_sphere_intersection(Vec3D center, double radius, const Ray& r, double t_0, double t_1) {
    // Get the A, B, C of the quadratic equation
    Vec3D OC = r.get_ray_origin() - center;
    auto A = r.get_ray_direction().length_squared();
    auto half_B = dot_product(OC, r.get_ray_direction());           // half_B is a shortcut
    auto C = OC.length_squared() - radius*radius;

    // Calculate the quadratic equation discriminant.
    auto discriminant = half_B * half_B - A * C;
    // auto discriminant = fma(half_B, half_B, -A * C);

    // If the discriminant is negative, the ray misses the sphere.
    if (discriminant < 0) return false;

    // Calculate the square root of the discriminant.
    double sqrt_discriminant = sqrt(discriminant);

    // Since t > 0 is part of the ray definition, we examine the two
    // roots. The smaller, positive real root is the one that is closest
    // to the intersection distance on the ray.
    double intersection_t = (-half_B - sqrt_discriminant) / A;       // first root
    if (intersection_t <= t_0 || t_1 <= intersection_t) {
        // first root not in range [t_0,t_1], so calculate
        // the second root.
        intersection_t = (-half_B + sqrt_discriminant) / A;

        // Check if second root is also not in the range [t_0,t_1].
        if (intersection_t <= t_0 || t_1 <= intersection_t)
            return false;
    }

    // We know the ray intersects the sphere, so we should update the
    // intersection information
    std::cout << "Correct Function - Intersection t: " << intersection_t << std::endl;
    std::cout << "Correct Function - Ray at t: " << r.at(intersection_t) << std::endl;

    return true;
}

bool new_sphere_intersection(Vec3D center, double radius, const Ray& r, double t_min, double t_max) {
    bool flag_outside_sphere = false;
    double R2 = radius*radius;
    Vec3D OC  = center - r.get_ray_origin();
    double L_2_OC = dot_product(OC, OC);
    flag_outside_sphere = false;
    if (L_2_OC >= R2) {
        // RAY ORIGINATED OURSIDE THE SPJERE
        flag_outside_sphere = true;
    }

    double t_ca = dot_product(OC, unit_vector(r.get_ray_direction()));

    if (t_ca < 0) {
        if (flag_outside_sphere)
            return false;
    }
    double t_2_hc = R2 - L_2_OC + (t_ca * t_ca);

    if (t_2_hc < 0) {
        return false;
    }

    std::cout << "1. " <<  (t_ca - sqrt(t_2_hc)) / r.get_ray_direction().length() << std::endl;
    std::cout << "2. " << (t_ca + sqrt(t_2_hc)) / r.get_ray_direction().length() << std::endl;

    double intersection_t;
    if (flag_outside_sphere)
        intersection_t =  (t_ca - sqrt(t_2_hc)) / r.get_ray_direction().length();
    else
        intersection_t = (t_ca + sqrt(t_2_hc)) / r.get_ray_direction().length();

    if (intersection_t <= t_min || t_max <= intersection_t) {
        return false;
    }
        std::cout << "New Function - Intersection at: " << intersection_t << std::endl;
        std::cout << "New Function - Ray at t: " << r.at(intersection_t) << std::endl;
        return true;

}

void test_sphere_intersection() {
    // Test case 1
    std::cout << "TEST 1: BEGIN" << std::endl;
    Ray r1(Vec3D(1, -2, -1), Vec3D(1, 2, 4));
    Sphere S1(Vec3D(3, 0, 5), 3, nullptr);
    correct_sphere_intersection(Vec3D(3, 0, 5), 3, r1, 0.001, infinity);
    new_sphere_intersection(Vec3D(3, 0, 5), 3, r1, 0.001, infinity);
    std::cout << "TEST 1: END\n" << std::endl;

    // Test case 2
    std::cout << "TEST 2: BEGIN" << std::endl;
    Ray r2(Vec3D(-3, 3, 5), Vec3D(1, 2, 4));
    Sphere S2(Vec3D(-3, 3, 5), 3, nullptr);
    correct_sphere_intersection(Vec3D(-3, 3, 5), 3, r2, 0.001, infinity);
    new_sphere_intersection(Vec3D(-3, 3, 5), 3, r2, 0.001, infinity);
    std::cout << "TEST 2: END\n" << std::endl;

    // Test case 3
    std::cout << "TEST 3: BEGIN" << std::endl;
    Ray r3(Vec3D(0.2, 0.5, 0.1), Vec3D(1, 2, 4));
    Sphere S3(Vec3D(0.2, 0.5, 0.1), 1, nullptr);
    correct_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r3, 0.001, infinity);
    new_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r3, 0.001, infinity);
    std::cout << "TEST 3: END\n" << std::endl;

    // Test case 4
    std::cout << "TEST 4: BEGIN" << std::endl;
    Ray r4(Vec3D(0.09, 0.03, 0.1), Vec3D(1, 2, 4));
    Sphere S4(Vec3D(0.2, 0.02, 0.1), 1, nullptr);
    correct_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r4, 0.001, infinity);
    new_sphere_intersection(Vec3D(0.2, 0.5, 0.1), 1, r4, 0.001, infinity);
    std::cout << "TEST 4: END\n" << std::endl;
}

bool correct_triangle_intersection(const Ray& r, point3D a, point3D b, point3D c, double t_0, double t_1) {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    Vec3D edge_1 = b - a;
    Vec3D edge_2 = c - a;
    Vec3D ray_cross_e2 = cross_product(r.get_ray_direction(), edge_2);
    double D = dot_product(edge_1, ray_cross_e2);

    // Front face culling
    if (D > -epsilon && D < epsilon)
        return false;  // parallel ray

    double inv_D = 1.0 / D;
    Vec3D s = r.get_ray_origin() - a;
    double u = inv_D * dot_product(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return false;

    Vec3D s_cross_e1 = cross_product(s, edge_1);
    double v = inv_D * dot_product(r.get_ray_direction(), s_cross_e1);

    if (v < 0 || u + v > 1)
        return false;

    double t = inv_D * dot_product(edge_2, s_cross_e1);

    if (t > epsilon) {
        // Ray intersects
        Vec3D n = cross_product(edge_1, edge_2);
        std::cout << "Correct Intersection Time: t = " << t << std::endl;
        std::cout << "Correct Normal: N = " << n;
        std::cout << "Correct Intersection Point: p = " << r.at(t) << std::endl;
        return true;
    } else {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }
}

bool new_triangle_intersection(const Ray& r, point3D a, point3D b, point3D c, double t_0, double t_1) {
    // Get Ray details
    // ----
    Vec3D e = r.get_ray_origin();
    Vec3D d = r.get_ray_direction();

    // Get Triangle's details
    Vec3D B_A = b - a;          // (b - a), an edge
    Vec3D C_A = c - a;           // (c - a), an edge

    // Set variable's for Cramer's Rule
    double A = a.x() - b.x();
    double D = a.x() - c.x();
    double G = d.x();

    double B = a.y() - b.y();
    double E = a.y() - c.y();
    double H  = d.y();

    double C = a.z() - b.z();
    double F = a.z() - c.z();
    double I = d.z();

    // Compute M
    double EI = E * I;
    double HF = H * F;

    double GF = G * F;
    double DI = D * I;

    double DH = D * H;
    double EG = E * G;

    double M = A * (EI - HF) + B * (GF - DI) + C * (DH - EG);

    // Compute t
    double J = a.x() - e.x();
    double K = a.y() - e.y();
    double L = a.z() - e.z();

    double AK = A * K;
    double JB = J * B;

    double JC = J * C;
    double AL = A * L;

    double BL = B * L;
    double KC = K * C;

    double t = -(F * (AK - JB) + E * (JC - AL) + D * (BL - KC)) / M;

    // Check for visibility in [t_0,t_1]
    if (t < t_0 || t > t_1) {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }

    // Compute GAMMA
    double GAMMA = (I * (AK - JB) + H * (JC - AL) + G * (BL - KC)) / M;

    // Check GAMMA's range
    if (GAMMA < 0 || GAMMA > 1) {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }

    // Compute BETA
    double BETA = (J * (EI - HF) + K * (GF - DI) + L * (DH - EG)) / M;

    // Check BETA's range
    if (BETA < 0 || BETA > 1 - GAMMA) {
        std::cout << "NO::INTERSECTION" << std::endl;
        return false;
    }
    // An intersection must happen
    if (t > epsilon) {
        Vec3D n = cross_product(B_A, C_A);
        std::cout << "New Function Intersection Time: t = " << t << std::endl;
        std::cout << "New Function Normal: N = " << n;
        std::cout << "New Function Intersection Point: p = " << r.at(t) << std::endl;
    }
    return true;
}

void test_triangle_intersection() {
    // Test case 1
    std::cout << "TEST 1: BEGIN" << std::endl;
    Ray r1(Vec3D(0,0,-2), Vec3D(0,0,1));
    correct_triangle_intersection(r1, point3D(-1, -1, 0), point3D(1,-1,0), point3D(0,1,0), 0.001, infinity);
    new_triangle_intersection(r1, point3D(-1, -1, 0), point3D(1,-1,0), point3D(0,1,0), 0.001, infinity);
    std::cout << "TEST 1: END\n" << std::endl;

    // Test case 2
    std::cout << "TEST 2: BEGIN" << std::endl;
    Ray r2(Vec3D(-3, 3, 5), Vec3D(1, 2, 4));
    correct_triangle_intersection(r2, point3D(0, 0, -1), point3D(0,1,0), point3D(1,0,0), 0.001, infinity);
    new_triangle_intersection(r2, point3D(0, 0, -1), point3D(0,1,0), point3D(1,0,0), 0.001, infinity);
    std::cout << "TEST 2: END\n" << std::endl;

    // Test case 3
    std::cout << "TEST 3: BEGIN" << std::endl;
    Ray r3(Vec3D(0, -2, 0), Vec3D(0, 1, 0));
    correct_triangle_intersection(r3, point3D(-1,0,0), point3D(0,1,0), point3D(1,1,0), 0.001, infinity);
    new_triangle_intersection(r3, point3D(-1,0,0), point3D(0,1,0), point3D(1,1,0), 0.001, infinity);
    std::cout << "TEST 3: END\n" << std::endl;

    // Test case 4
    std::cout << "TEST 4: BEGIN" << std::endl;
    Ray r4(Vec3D(0.09, 0.03, 0.1), Vec3D(1, 2, 4));
    correct_triangle_intersection(r4, point3D(0,0,0), point3D(1,0,0), point3D(0,1,0), 0.001, infinity);
    new_triangle_intersection(r4, point3D(0,0,0), point3D(1,0,0), point3D(0,1,0), 0.001, infinity);
    std::cout << "TEST 4: END\n" << std::endl;
}
int main() {
    auto start = omp_get_wtime();

 //   Scene_Information scene_info = scene_Test();
    Scene_Information scene_info = bunny_test(); //bunny_test();

  //  parallel_render_radiance_mixture(scene_info);      //parallel
    //serial_render(scene_info);



    parallel_render_radiance_background(scene_info);

    auto stop = omp_get_wtime();

    auto duration = stop - start;

    std::cout << duration << std::endl;

    scene_info.rendering_time = duration;

    //test_sphere_intersection();
    //test_triangle_intersection();


    return 0;
}
