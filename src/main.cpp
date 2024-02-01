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


/* CORRECT LOOPS, JUST NEED FIXING SCENE_INFO
void rendering_loop(int samples_per_pixel, const std::string& output_image_name) {
    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 10;

    // World
    // -------------------------------------------------------------------------------
    Primitives_Group world = many_objects_scene();


    // Camera
    // -------------------------------------------------------------------------------
    //  point3D lookfrom(0, 15, 0); // Adjust position for a wide-angle view
    //  point3D lookat(5, -20, 0); // Focus on the central part of the scene
    //  Vec3D vup(0, 1, 0);
    //  auto vfov = 90.0; // Wider field of view for a panorama


    point3D lookfrom(5, 5, 10);
    point3D lookat(0, 0, 0);
    Vec3D vup(0, 1, 0);
    auto vfov = 30.0;
    point3D lookfrom(13,2,3);
    point3D lookat(0, 0, 0);
    Vec3D vup(0, 1, 0);
    double vfov     = 20;

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
    //    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
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
*/

/*
void parallel_rendering_loop(int samples_per_pixel, const std::string& output_image_name) {
    // Image
    // -------------------------------------------------------------------------------
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 10;

    // World
    // -------------------------------------------------------------------------------
    //Primitives_Group world = many_objects_scene();
    //Primitives_Group world = many_objects_scene();
    Primitives_Group world = diffuse_ambient_scene_2();

    // Camera
    // -------------------------------------------------------------------------------
    point3D lookfrom(13,2,3);
    point3D lookat(0, 0, 0);
    Vec3D vup(0, 1, 0);
    double vfov     = 20;
    /* Was used for the final image of the dissertation
    point3D lookfrom(5, 5, 5);
    point3D lookat(0, 0, 0);
    Vec3D vup(0, 1, 0);
    auto vfov = 30.0;



    point3D lookfrom(0, 15, 0); // Adjust position for a wide-angle view
    point3D lookat(5, -20, 0); // Focus on the central part of the scene
    Vec3D vup(0, 1, 0);
    auto vfov = 90.0; // Wider field of view for a panorama */

// Use for final scene
 //   point3D lookfrom(5, 5, 30);
 //   point3D lookat(0, 0, 0);
 //   Vec3D vup(0, 1, 0);
 //   auto vfov = 30.0;

/*
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

#pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, cam, world, pixel_colors) num_threads(16)
    for (int j = image_height - 1; j >=0; --j) {
       // std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        #pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, j, cam, world, pixel_colors) num_threads(16)
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
             #pragma omp parallel for schedule(dynamic) default(none) shared(samples_per_pixel, i, j, cam, pixel_color, world, image_width, image_height) num_threads(16)
            for (int s = 0; s < samples_per_pixel; ++s) {
                //std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
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
            // Output the averaged color to the PPM file
            ofs << static_cast<int>(255 * clamp(pixel_color.x(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.y(), 0.0, 0.999)) << ' '
                << static_cast<int>(255 * clamp(pixel_color.z(), 0.0, 0.999)) << '\n';
        }
    }
}
*/

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

     std::cout << "Image height = " << image_height << std::endl;
     std::cout << "Image Width = " << image_width << std::endl;
     std::cout << "Depth = " << max_depth << std::endl;
     std::cout << "Samples-per-pixel = " << samples_per_pixel << std::endl;
     // Render Loop
     // -----------------------------------------------------------------------
     // Reference: [3] How to write to a PPM file? https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++
     ofs << "P3\n" << image_width << " " << image_height << "\n255\n";
     std::vector<std::vector<Color>> pixel_colors(image_height, std::vector<Color>(image_width, Color(0, 0, 0)));

     #pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, cam, world, pixel_colors, max_depth, lights) num_threads(16)
     for (int j = image_height - 1; j >=0; --j) {
         // std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
         #pragma omp parallel for default(none) shared(samples_per_pixel, image_height, image_width, j, cam, world, lights, pixel_colors, max_depth) num_threads(16)
         for (int i = 0; i < image_width; ++i) {
             Color pixel_color(0.0, 0.0, 0.0);   // Initialize pixel color
             #pragma omp parallel for schedule(dynamic) default(none) shared(samples_per_pixel, i, j, cam, pixel_color, world, lights, image_width, image_height, max_depth) num_threads(16)
             for (int s = 0; s < samples_per_pixel; ++s) {
              //   std::cout << "Number of active threads = " << omp_get_thread_num() << std::endl;
                 auto u = (i + random_double()) / (image_width - 1);
                 auto v = (j + random_double()) / (image_height - 1);

                 // Construct a ray from the camera origin in the direction of the sample point
                 Ray r = cam.get_ray(u, v);

                 // Accumulate color for each sample
                 pixel_color +=  radiance_background(r, world, max_depth);
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

int main() {
    /* Reference:   The math for the image and camera details was inspired from:
     * https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
     */


    auto start = omp_get_wtime();
   // rendering_loop(3, "Translation Test");        old but working, need update with scene_info
  //  parallel_rendering_loop(5, "Am i dsiplaced");    // old but working, need update with scene_infp

    // working:
 //   Scene_Information scene_info = scene_Test();
    Scene_Information scene_info = many_balls_scene();
     render(scene_info);      //parallel
    //serial_render(scene_info);
    auto stop = omp_get_wtime();
    auto duration = stop - start;
    std::cout << duration << std::endl;
    scene_info.rendering_time = duration;
    return 0;
}
