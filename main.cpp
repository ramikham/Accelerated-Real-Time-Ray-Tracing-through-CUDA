#include "Utilities.h"
#include "Camera.h"
#include "Primitives/Sphere.h"
#include "Primitives/Primitives_Group.h"
#include "Shading.h"
#include "Scenes.h"

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
            Color pixel_color = cast_ray(r, first_scene());

            // Output the color the PPM file
            ofs << static_cast<int>(255.9 * pixel_color.x()) << ' '
                << static_cast<int>(255.9 * pixel_color.y()) << ' '
                << static_cast<int>(255.9 * pixel_color.z()) << '\n';
        }
    }
}

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
            for (int s = 0; s < samples_per_pixel; ++s) {
                // Calculate random offsets within the pixel
                double u_offset = (i+random_double()) / (cam.image_width - 1);
                double v_offset = (j+random_double()) / (cam.image_height - 1);

                // Calculate the ray direction from the camera origin to the sample point
                point3D ray_direction = cam.viewport_upper_left_corner + u_offset * cam.viewport_u + v_offset * cam.viewport_v - cam.camera_origin;

                // Construct a ray from the camera origin in the direction of the sample point
                Ray r(cam.camera_origin, ray_direction);

                // Accumulate color for each sample
                pixel_color += cast_ray(r, first_scene(), 50);
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

int main() {
    /* Reference:   The math for the image and camera details was inspired from:
     * https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
     */

    // Image Details
    // -----------------------------------------------------------------------
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 800;
    int image_height = static_cast<int>(image_width/aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera Details
    // -----------------------------------------------------------------------
    point3D camera_origin = point3D(0,0,0);
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

    Vec3D viewport_u = Vec3D(viewport_width, 0, 0);
    Vec3D viewport_v = Vec3D(0, -viewport_height, 0);

    Vec3D pixel_delta_u = viewport_u / image_width;
    Vec3D pixel_delta_v = viewport_v / image_height;

    Vec3D viewport_upper_left_corner = camera_origin - Vec3D(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    Vec3D first_pixel_center_location = viewport_upper_left_corner + 0.5 * (pixel_delta_u + pixel_delta_v);

    Camera cam(camera_origin, aspect_ratio, focal_length, image_width, viewport_height);
    //rendering_loop(cam, "No Multisampling");
    rendering_loop_with_supersampling(200, cam, "Test Specular - New Scene");
    return 0;
}
