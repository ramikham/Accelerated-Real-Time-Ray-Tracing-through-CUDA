//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SCENES_H
#define CUDA_RAY_TRACER_SCENES_H

#include "Utilities.h"
#include "Primitives/Primitives_Group.h"
#include "Primitives/Sphere.h"
#include "Materials/Diffuse.h"
#include "Materials/Specular.h"
#include "Accelerators/BVH.h"
#include "Accelerators/BVH_Max_Coordinate.h"
#include "Accelerators/BVH_Centroid_Coordinate.h"
#include "Accelerators/BVH_Parallel.h"
#include "Mathematics/Transformations/Translate.h"
#include "Materials/Phong.h"
#include "Materials/Diffuse_Light.h"
#include "Mathematics/Transformations/Rotate_Y.h"
#include "Mathematics/Transformations/Rotate_Z.h"
#include "Mathematics/Transformations/Rotate_X.h"
#include "Mathematics/Transformations/Translate.h"
#include "Primitives/XY_Rectangle.h"
#include "Primitives/YZ_Rectangle.h"
#include "Primitives/XZ_Rectangle.h"
#include "Primitives/Box.h"
#include "Materials/Uniform_Hemispherical_Diffuse.h"
#include "Primitives/Triangle.h"
#include "Accelerators/BVH_Fast.h"
#include "Textures/Texture.h"
#include "Cameras/Camera.h"
#include "Materials/Diffuse_With_Texture.h"
#include "Materials/Disney_Diffuse.h"

struct Scene_Information {
    // Image settings
    // -------------------------------------------------------------------------------
    double aspect_ratio;
    int image_width;
    int image_height;

    // Rendering settings
    // -------------------------------------------------------------------------------
    int max_depth;
    int samples_per_pixel;
    Primitives_Group world;
    Primitives_Group lights;

    // Camera settings
    // -------------------------------------------------------------------------------
    Vec3D lookfrom;
    Vec3D lookat;
    Vec3D vup;
    double vfov;

    Camera camera;
    Orthographic_Camera orthographic_camera;

    // .ppm image file name
    // -------------------------------------------------------------------------------
    std::string output_image_name;

    // Statistics
    // -------------------------------------------------------------------------------
    double BVH_build_time;
    double render_time;
    int number_of_ray_intersection_tests;
    int number_of_threads_used;
};

Scene_Information one_weekend_scene() {
    /// Reference: Ray Tracing in One Weekend - https://raytracing.github.io/books/RayTracingInOneWeekend.html#wherenext?/afinalrender
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 16.0 / 9.0;
    scene_info.image_width = 1200;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 2000;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(13, 2, 3);
    scene_info.lookat = Vec3D(0,0,0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 20;       // use 80 for debugging
    scene_info.output_image_name = "One Weekend Scene";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> ground_material = std::make_shared<Diffuse>(Color(0.5, 0.5, 0.5));
    std::shared_ptr<Specular> material_1 = std::make_shared<Specular>(Color(0.81, 0.85, 0.88), 0.0, 1.0);
    std::shared_ptr<Diffuse> material_2 = std::make_shared<Diffuse>(Color(0.4, 0.2, 0.1));
    std::shared_ptr<Specular> material_3 = std::make_shared<Specular>(Color(0.7, 0.6, 0.5), 0.0, 1.0);

    // Primitives
    // -------------------------------------------------------------------------------
    // Add balls
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0, -1000, 0), 1000, ground_material));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0, 1, 0), 1.0, material_1));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(-4, 1, 0), 1.0, material_2));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(4, 1, 0), 1.0, material_3));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3D center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3D(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;
                if (choose_mat < 0.8) {
                    // diffuse
                    Vec3D albedo = random_vector_in_range() * random_vector_in_range();
                    sphere_material = std::make_shared<Diffuse>(albedo);
                    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    Vec3D albedo = random_vector_in_range(0.5, 1);
                    double fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Specular>(albedo, fuzz, 1.0);
                    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(center, 0.2, material_1));
                }
            }
        }
    }

    // Construct BVH
    // -------------------------------------------------------------------------------
    double start = omp_get_wtime();
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));
    double end = omp_get_wtime();
    scene_info.BVH_build_time = end - start;

    return scene_info;
}

Scene_Information enter_the_dragon() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 20;
    scene_info.samples_per_pixel = 1;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;       // use 80 for debugging
    scene_info.output_image_name = "Enter the Dragon";          //NO ISAMP

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);
    // Materials
    // -------------------------------------------------------------------------------

    std::shared_ptr<Phong> royal_blue_phong = std::make_shared<Phong>(Color(0.25, 0.41, 1.0), 0.8, 2.5);
    std::shared_ptr<Phong> red_phong = std::make_shared<Phong>(Color(0.65, 0.05, 0.05), 0.8, 2.5);
    std::shared_ptr<Phong> sky_blue_phong = std::make_shared<Phong>(Color(0.70, 0.80, 1.00), 0.8, 2.5);
    std::shared_ptr<Phong> green_phong = std::make_shared<Phong>(Color(0.12, 0.45, 0.15), 0.8, 2.5);

    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green_phong));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red_phong));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), royal_blue_phong));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), sky_blue_phong));

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Stanford Dragon */
    /******************/

    // Dragon's material
    std::shared_ptr<Phong> gold_phong = std::make_shared<Phong>(Color(0.83, 0.87, 0.22), 0.8, 2.5);

    std::vector<point3D> bunny_vertices;
    std::vector<Triangle> bunny_faces;

    // Dragon's displacement vector
    Vec3D bunny_D = Vec3D(40, 120, 250);

    // Dragon's scale factor
    double bunny_scale_factor = 2000;

    // Dragon's Rotation
    double angle_of_rotation_X = 0; double angle_of_rotation_Y = 180; double angle_of_rotation_Z = 0;

    // Load the Stanford Dragon from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\dragon.obj" , bunny_vertices, bunny_faces, gold_phong, bunny_D, bunny_scale_factor, angle_of_rotation_X, angle_of_rotation_Y, angle_of_rotation_Z); // "C:\\Users\\Rami\\Desktop\\Lucy.obj"

    // Add the dragon to the world
    for (const auto& triangle : bunny_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    auto start = omp_get_wtime();           // measure time

    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

Scene_Information Lucy_with_light() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 1000;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "Lucy with Light";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);
    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> light_green = std::make_shared<Diffuse>(Color(0.2,0.9,0.1));
    std::shared_ptr<Diffuse> sphere_blue = std::make_shared<Diffuse>(Color(0.1, 0.3, 0.5));
    std::shared_ptr<Diffuse_Light> sphere_light = std::make_shared<Diffuse_Light>(Color(0.1, 0.3, 1));

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), light_green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(190,90,190), 90, sphere_light));

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Stanford Lucy */
    /******************/

    // Lucy's material
    std::shared_ptr<Diffuse> lucy_mat = std::make_shared<Diffuse>(Color(0.5,0.5,0.5));

    std::vector<point3D> lucy_vertices;
    std::vector<Triangle> lucy_faces;

    // Lucy's displacement vector
    Vec3D bunny_D = Vec3D(290, 125, 160);

    // Lucy's scale factor
    double bunny_scale_factor = 2400;

    // Lucy's angle of rotation
    double angle_of_rotation_X = 0; double angle_of_rotation_Y = 180; double angle_of_rotation_Z = 0;

    // Load the Stanford Lucy from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Lucy.obj", lucy_vertices, lucy_faces, lucy_mat, bunny_D, bunny_scale_factor, angle_of_rotation_X, angle_of_rotation_Y, angle_of_rotation_Z);

    // Add Lucy faces to the world
    for (const auto& triangle : lucy_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<Sphere>(point3D(190,90,190), 90, m));

    return scene_info;
}

Scene_Information enter_Lucy() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 10;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "Lucy";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> sky_blue  = std::make_shared<Diffuse>(Color(0.70, 0.80, 1.00));
    std::shared_ptr<Diffuse> orange = std::make_shared<Diffuse>(Color(1,0.5,0));
    std::shared_ptr<Diffuse> pink = std::make_shared<Diffuse>(Color(1,0.4,0.7));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));
    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), orange));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), sky_blue));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Lucy */
    /******************/

    // Lucy's material
    std::shared_ptr<Diffuse> lucy_mat = std::make_shared<Diffuse>(Color(0.5,0.5,0.5));      // Lucy's material is grey

    std::vector<point3D> lucy_vertices;
    std::vector<Triangle> lucy_faces;

    // Lucy's displacement vector
    Vec3D lucy_D = Vec3D(40, 125, 250);

    // Lucy's scale factor
    double lucy_scale_factor = 2400;

    // Lucy's rotation angles
    double angle_of_rotation_X = 0;
    double angle_of_rotation_Y = 180;
    double angle_of_rotation_Z = 0;

    // Load Lucy from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Lucy.obj", lucy_vertices, lucy_faces, lucy_mat, lucy_D, lucy_scale_factor, angle_of_rotation_X, angle_of_rotation_Y, angle_of_rotation_Z);

    // Add Lucy's faces to the world
    for (const auto& triangle : lucy_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    /* Utah Teapot */
    /******************/

    // Load the OBJ file
    std::vector<point3D> teapot_vertices;
    std::vector<Triangle> teapot_triangles;

    // Teapot's displacement vector
    Vec3D teapot_D = Vec3D(400.0, -0.5, 120.0);

    // Teapot's scale factor
    double teapot_scale_factor = 40.0;

    // Teapot's angle of rotation
    double teapot_angle_of_rotation = -59.5;

    // Teapot's material
    // We will use pink, which is defined above

    // Load the teapot from the .obj file
    load_model("C:\\Users\\Rami\\Downloads\\teapot.obj", teapot_vertices, teapot_triangles, pink, teapot_D, teapot_scale_factor, teapot_angle_of_rotation);

    // Add the teapot's faces to the world
    for (const auto& triangle : teapot_triangles) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    auto start = omp_get_wtime();           // measure time
    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;
    scene_info.BVH_build_time = end - start;

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

Scene_Information Lucy_with_a_mirror() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 10;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;       // use 80 for debugging
    scene_info.output_image_name = "Tst";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);
    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> orange = std::make_shared<Diffuse>(Color(0.035, 0.247,0.18));
    std::shared_ptr<Diffuse> pink = std::make_shared<Diffuse>(Color(1,0.4,0.7));
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));
    std::shared_ptr<Diffuse> lucy_mat = std::make_shared<Diffuse>(Color(0.27,0.34,0.47));      // Lucy's material is chambray https://www.picmonkey.com/colors/blue/chambray
    std::shared_ptr<Specular> mirror = std::make_shared<Specular>(Color(0.81, 0.85, 0.88), 0.0, 1.0);


    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));


    // Add the boxes
    std::shared_ptr<Primitive> box1 = std::make_shared<Box>(point3D(0,0,0), point3D(165,390,165), mirror);

    // Translation/Rotation
    // -------------------------------------------------------------------------------
    box1 = std::make_shared<Rotate_Y>(box1, 27);
    box1 = std::make_shared<Translate>(box1, Vec3D(265,0,295));
    scene_info.world.add_primitive_to_list(box1);

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Lucy */
    /******************/
    std::vector<point3D> lucy_vertices;
    std::vector<Triangle> lucy_faces;

    // Lucy's displacement vector
    Vec3D bunny_D = Vec3D(60, 125, 55);               // y was 105 when x rotation was -58.7      (y=130 when xrotation = -58.5)

    // Lucy's scale factor
    double bunny_scale_factor = 2400;       // was 0.25

    // Lucy's rotation angles
    double angle_of_rotation_X = 0;
    double angle_of_rotation_Y = 180;
    double angle_of_rotation_Z = 0;

    // Load Lucy from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Lucy.obj", lucy_vertices, lucy_faces, lucy_mat, bunny_D, bunny_scale_factor, angle_of_rotation_X, angle_of_rotation_Y, angle_of_rotation_Z);

    // Add Lucy's faces to the world
    for (const auto& triangle : lucy_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

Scene_Information a_rabbit_and_a_teapot_inside_a_Cornell_box() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 10;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "Rabbit and Teapot in a Room";          //NO ISAMP

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));
    std::shared_ptr<Diffuse> blue = std::make_shared<Diffuse>(Color(0.7,0.7, 1.0));
    std::shared_ptr<Diffuse> orange = std::make_shared<Diffuse>(Color(1,0.5,0));
    std::shared_ptr<Diffuse> pink = std::make_shared<Diffuse>(Color(1,0.4,0.7));

    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    // Add the boxes
    std::shared_ptr<Primitive> box1 = std::make_shared<Box>(point3D(0,0,0), point3D(165,330,165), orange);
    std::shared_ptr<Primitive> box2 = std::make_shared<Box>(point3D(0,0,0), point3D(165,165,165), pink);

    // Translation/Rotation
    // -------------------------------------------------------------------------------
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3D(265,0,295));
    scene_info.world.add_primitive_to_list(box1);
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3D(90,0,65));
    scene_info.world.add_primitive_to_list(box2);
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Stanford Bunny */
    /******************/

    // Bunny's material
    std::shared_ptr<Diffuse> bunny_material = std::make_shared<Diffuse>(Color(0.9, 0.6, 0.4));

    std::vector<point3D> bunny_vertices;
    std::vector<Triangle> bunny_faces;

    // Bunny's displacement vector
    Vec3D bunny_D = Vec3D(130, 140, 150);

    // Bunny's scale factor
    double bunny_scale_factor = 700.0;

    // Bunny's angle of rotation
    double angle_of_rotation_1 = -59.5;

    // Load the Stanford Bunny from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Folders\\Computer Graphics\\3D Models\\Simple_Bunny.obj.txt", bunny_vertices, bunny_faces, bunny_material, bunny_D, bunny_scale_factor, angle_of_rotation_1);

    // Add the bunny faces to the world
    for (const auto& triangle : bunny_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    /* Utah Teapot */
    /******************/
    // Load the OBJ file
    std::vector<point3D> teapot_vertices;
    std::vector<Triangle> teapot_triangles;

    // Teapot's displacement vector
    Vec3D D = Vec3D(400.0, -0.5, 210.0);

    // Teapot's scale factor
    double scale_factor = 40.0;

    // Teapot's angle of rotation
    double angle_of_rotation = -59.5;

    // Teapot's material
    //std::shared_ptrstd::shared_ptr<Phong> teapot_royal_blue_phong = std::make_shared<Phong>(Color(0.25, 0.41, 1.0), 0.8, 2.5);
    auto teapot_royal_blue_phong = std::make_shared<Diffuse>(Color(0.25, 0.41, 1.0));

    // Load the model from the .obj file
    load_model("C:\\Users\\Rami\\Downloads\\teapot.obj", teapot_vertices, teapot_triangles, teapot_royal_blue_phong, D, scale_factor, angle_of_rotation);

    // Add the teapot's faces to the world
    for (const auto& triangle : teapot_triangles) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    auto start = omp_get_wtime();           // measure time
    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));
    // scene_info.world = Primitives_Group(std::make_shared<BVH>(scene_info.world));
    // scene_info.world = Primitives_Group(std::make_shared<BVH_Max_Coordinate>(scene_info.world));
    // scene_info.world = Primitives_Group(std::make_shared<BVH_Centroid_Coordinate>(scene_info.world));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;
    scene_info.BVH_build_time = end - start;

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

Scene_Information a_rabbit_and_a_teapot_inside_a_Cornell_box_without_importance_sampling() {
    auto start = omp_get_wtime();           // measures BVH construction time

    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Render settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 10;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "No Importance Smapling";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Uniform_Hemispherical_Diffuse> red = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Uniform_Hemispherical_Diffuse> white = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Uniform_Hemispherical_Diffuse> green = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.12, 0.45, 0.15));
    std::shared_ptr<Uniform_Hemispherical_Diffuse> blue = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.7,0.7, 1.0));
    std::shared_ptr<Uniform_Hemispherical_Diffuse> orange = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(1,0.5,0));
    std::shared_ptr<Uniform_Hemispherical_Diffuse> pink = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(1,0.4,0.7));

    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Primitives
    // -------------------------------------------------------------------------------
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    std::shared_ptr<Primitive> box1 = std::make_shared<Box>(point3D(0,0,0), point3D(165,330,165), orange);
    std::shared_ptr<Primitive> box2 = std::make_shared<Box>(point3D(0,0,0), point3D(165,165,165), pink);

    // Translation/Rotation
    // -------------------------------------------------------------------------------
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3D(265,0,295));
    scene_info.world.add_primitive_to_list(box1);

    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3D(90,0,65));
    scene_info.world.add_primitive_to_list(box2);

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Stanford Bunny */
    /******************/

    // Load the OBJ file
    std::vector<point3D> bunny_vertices;
    std::vector<Triangle> bunny_faces;

    // Displacement vector
    Vec3D bunny_D = Vec3D(130, 140, 150);

    // Scale factor
    double bunny_scale_factor = 700.0;

    // Angle of rotation
    double bunny_angle_of_rotation = -59.5;

    // Material
    std::shared_ptr<Uniform_Hemispherical_Diffuse> bunny_material = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.9, 0.6, 0.4));

    // Load the model from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Folders\\Computer Graphics\\3D Models\\Simple_Bunny.obj.txt", bunny_vertices, bunny_faces, bunny_material, bunny_D, bunny_scale_factor, bunny_angle_of_rotation);

    // Add bunny's triangles to the scene's "world"
    for (const auto& triangle : bunny_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    /* Utah Teapot */
    /******************/

    // Load the OBJ file
    std::vector<point3D> teapot_vertices;
    std::vector<Triangle> teapot_faces;

    // Displacement vector
    Vec3D D = Vec3D(400.0, -0.5, 210.0);

    // Scale factor
    double scale_factor = 40.0;

    // Angle of rotation
    double angle_of_rotation = -59.5;

    // Material
    std::shared_ptr<Phong> royal_blue_phong = std::make_shared<Phong>(Color(0.25, 0.41, 1.0), 0.8, 2.5);

    // Load the model from the .obj file
    load_model("C:\\Users\\Rami\\Downloads\\teapot.obj", teapot_vertices, teapot_faces, royal_blue_phong, D, scale_factor, angle_of_rotation);

    // Add triangles to the scene's "world"
    for (const auto& triangle : teapot_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;

    return scene_info;
}

Scene_Information full_Cornell_box() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 3000;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "erato - 3000 SPP";          //NO ISAMP

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));
    std::shared_ptr<Diffuse> blue = std::make_shared<Diffuse>(Color(0.7,0.7, 1.0));
    std::shared_ptr<Diffuse> orange = std::make_shared<Diffuse>(Color(1,0.5,0));
    std::shared_ptr<Diffuse> green_shade = std::make_shared<Diffuse>(Color(0.86,0.91,0.85));

    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    // Add the boxes
    std::shared_ptr<Primitive> box = std::make_shared<Box>(point3D(0, 0, 0), point3D(165, 165, 165), green_shade);

    // Translation/Rotation
    // -------------------------------------------------------------------------------
    box = std::make_shared<Rotate_Y>(box, -18);
    box = std::make_shared<Translate>(box, Vec3D(90, 0, 65));
    scene_info.world.add_primitive_to_list(box);
    scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------

    /* Erato */
    /******************/

    // Erato's material
    std::shared_ptr<Diffuse> erato_material = std::make_shared<Diffuse>(Color(0.77, 0.58, 0.29));

    std::vector<point3D> erato_vertices;
    std::vector<Triangle> erato_faces;

    // Erato's displacement vector
    Vec3D erato_D = Vec3D(300, 0, 300);

    // Erato's scale factor
    double erato_scale_factor = 40.0;

    double xa = 0; double ya = 180; double xc = 0;          //SETUP GOOD NOW CHANGE VFOV

    // Load the Stanford Bunny from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Erato.obj" , erato_vertices, erato_faces, erato_material, erato_D, erato_scale_factor, xa, ya, xc); // "C:\\Users\\Rami\\Desktop\\Lucy.obj"

    // Add the bunny faces to the world
    for (const auto& triangle : erato_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    /* Stanford Bunny */
    /******************/
    // Bunny's material
    std::shared_ptr<Diffuse> pink = std::make_shared<Diffuse>(Color(1,0.4,0.7));

    std::vector<point3D> bunny_vertices;
    std::vector<Triangle> bunny_faces;

    // Bunny's displacement vector
    Vec3D bunny_D = Vec3D(130, 140, 150);

    // Bunny's scale factor
    double bunny_scale_factor = 700.0;

    // Bunny's angle of rotation
    double angle_of_rotation_1 = -59.5;

    // Load the Stanford Bunny from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Folders\\Computer Graphics\\3D Models\\Simple_Bunny.obj.txt", bunny_vertices, bunny_faces, pink, bunny_D, bunny_scale_factor, angle_of_rotation_1);
    // Add the bunny faces to the world
    for (const auto& triangle : bunny_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    /* Utah Teapot */
    /******************/
    // Load the OBJ file
    std::vector<point3D> teapot_vertices;
    std::vector<Triangle> teapot_triangles;

    // Teapot's displacement vector
    Vec3D D = Vec3D(400.0, -0.5, 210.0);

    // Teapot's scale factor
    double scale_factor = 40.0;

    // Teapot's angle of rotation
    double angle_of_rotation = -59.5;

    // Teapot's material
    //std::shared_ptrstd::shared_ptr<Phong> teapot_royal_blue_phong = std::make_shared<Phong>(Color(0.25, 0.41, 1.0), 0.8, 2.5);
    auto teapot_royal_blue_phong = std::make_shared<Diffuse>(Color(0.25, 0.41, 1.0));

    // Load the model from the .obj file
    load_model("C:\\Users\\Rami\\Downloads\\teapot.obj", teapot_vertices, teapot_triangles, teapot_royal_blue_phong, D, scale_factor, angle_of_rotation);

    // Add the teapot's faces to the world
    for (const auto& triangle : teapot_triangles) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    auto start = omp_get_wtime();           // measure time
    // Construct BVH
    // -------------------------------------------------------------------------------
    // scene_info.world = Primitives_Group(std::make_shared<BVH_Fast>(scene_info.world));
    scene_info.world = Primitives_Group(std::make_shared<BVH_Parallel>(scene_info.world));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;
    scene_info.BVH_build_time = end - start;

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

Scene_Information texture_Cornell_box() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 30;
    scene_info.samples_per_pixel = 4000;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "Stripes Texture Scene";          //NO ISAMP

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));

    // Light
    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Textures
    // -------------------------------------------------------------------------------
    Color gold_color = Color(1.0,0.84,0);
    std::shared_ptr<Constant_Color> gold_color_texture = std::make_shared<Constant_Color>(gold_color);

    Color burgandy_color = Color(0.5, 0, 0.13);
    std::shared_ptr<Constant_Color> different_green_color_texture = std::make_shared<Constant_Color>(burgandy_color);

    // Stripes Texture
    std::shared_ptr<Stripe_Texture_Controllable_Width> stripes_2 = std::make_shared<Stripe_Texture_Controllable_Width>(gold_color_texture, different_green_color_texture, 20, true);
    std::shared_ptr<Diffuse_With_Texture> diffuse_texture_2 = std::make_shared<Diffuse_With_Texture>(stripes_2);

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    // Add Meshes to the scene
    // -------------------------------------------------------------------------------
    /* Stanford Dragon */
    /******************/

    std::vector<point3D> bunny_vertices;
    std::vector<Triangle> bunny_faces;

    // Dragon's displacement vector
    Vec3D bunny_D = Vec3D(40, 120, 250);

    // Dragon's scale factor
    double bunny_scale_factor = 2000;

    // Dragon's Rotation
    double angle_of_rotation_X = 0; double angle_of_rotation_Y = 180; double angle_of_rotation_Z = 0;

    // Load the Stanford Dragon from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\dragon.obj" , bunny_vertices, bunny_faces, diffuse_texture_2, bunny_D, bunny_scale_factor, angle_of_rotation_X, angle_of_rotation_Y, angle_of_rotation_Z); // "C:\\Users\\Rami\\Desktop\\Lucy.obj"

    // Add the dragon to the world
    for (const auto& triangle : bunny_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    auto start = omp_get_wtime();           // measure time
    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Parallel>(scene_info.world));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;
    scene_info.BVH_build_time = end - start;

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

Scene_Information different_diffuse_models_scene() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 30;
    scene_info.samples_per_pixel = 4000;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "Three Diffuse Spheres";          //NO ISAMP

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    // Walls materials
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));

    // Spheres materials
    std::shared_ptr<Diffuse> sphere_blue_diffuse = std::make_shared<Diffuse>(Color(0.70, 0.80, 1.00));
    std::shared_ptr<Uniform_Hemispherical_Diffuse> sphere_blue_hemi_diffuse = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.70, 0.80, 1.00));
    std::shared_ptr<Disney_Diffuse> sphere_blue_disney_diffuse = std::make_shared<Disney_Diffuse>(Color(0.70, 0.80, 1.00), 0.7);

    // Light material
    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(30,30,30));

    // Primitives
    // -------------------------------------------------------------------------------
    // Add walls and light in the ceiling
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));

    // Add the three spheres
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(150,60,190), 60, sphere_blue_diffuse));       // right sphere
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(285,60,190), 60, sphere_blue_hemi_diffuse));       // middle sphere
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(420,60,190), 60, sphere_blue_disney_diffuse)); // left sphere

    auto start = omp_get_wtime();           // measure time
    // Construct BVH
    // -------------------------------------------------------------------------------
    scene_info.world = Primitives_Group(std::make_shared<BVH_Parallel>(scene_info.world));

    auto end = omp_get_wtime();
    std::cout << "BVH Building took: " <<  end - start << std::endl;
    scene_info.BVH_build_time = end - start;

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));

    return scene_info;
}

#endif //CUDA_RAY_TRACER_SCENES_H