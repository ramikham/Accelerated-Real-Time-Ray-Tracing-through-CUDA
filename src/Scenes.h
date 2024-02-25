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
#include "Accelerators/BVH_Midpoint_Partition.h"
#include "Accelerators/Parallel_Bounding_Volume_Hierarchy.h"
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
#include "Accelerators/BVH_Midpoint_Partition.h"
#include "Primitives/Triangle.h"
#include "Accelerators/BVH_Fast.h"
#include "Accelerators/BVH_New.h"

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

    // .ppm image file name
    // -------------------------------------------------------------------------------
    std::string output_image_name;

    // Statistics
    // -------------------------------------------------------------------------------
    double rendering_time;
    int number_of_ray_intersection_tests;
    int number_of_threads_used;
};

Scene_Information a_rabbit_and_a_teapot_inside_a_Cornell_box_with_importance_sampling() {
    /*  Note: This scene was used to render figure XXX. To see the effects of importance sampling,
            render the scene with radiance_mixture() and choose the importance sampling evaluate()
            version of the materials used in the scene.
    */

    auto start = omp_get_wtime();           // measure time

    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 800;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 500;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(278, 278, -800);
    scene_info.lookat = Vec3D(278, 278, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 40;
    scene_info.output_image_name = "A Teapot and a Rabbit Inside a Cornell Box - NO ISP - 500 SPP";          //NO ISAMP

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
    std::shared_ptr<Phong> teapot_royal_blue_phong = std::make_shared<Phong>(Color(0.25, 0.41, 1.0), 0.8, 2.5);

    // Load the model from the .obj file
    load_model("C:\\Users\\Rami\\Downloads\\teapot.obj", teapot_vertices, teapot_triangles, teapot_royal_blue_phong, D, scale_factor, angle_of_rotation);

    // Add the teapot's faces to the world
    for (const auto& triangle : teapot_triangles) {
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


Scene_Information a_rabbit_and_a_teapot_inside_a_Cornell_box_without_importance_sampling() {
    /*  Note: This scene was used to render figure XXX. To see the effects of no importance sampling,
            render the scene with radiance_background() and choose the non-importance sampling evaluate()
            version of the materials used in the scene.
    */

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
    scene_info.output_image_name = "A TEST";          //NO ISAMP

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
    Vec3D D_1 = Vec3D(130, 140, 150);

    // Scale factor
    double bunny_scale_factor = 700.0;

    // Angle of rotation
    double bunny_angle_of_rotation = -59.5;

    // Material
    std::shared_ptr<Uniform_Hemispherical_Diffuse> bunny_material = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.9, 0.6, 0.4));

    // Load the model from the .obj file
    load_model("C:\\Users\\Rami\\Desktop\\Folders\\Computer Graphics\\3D Models\\Simple_Bunny.obj.txt", bunny_vertices, bunny_faces, bunny_material, D_1, bunny_scale_factor, bunny_angle_of_rotation);

    // Add bunny's triangles to the scene's "world"
    for (const auto& triangle : bunny_faces) {
        scene_info.world.add_primitive_to_list(std::make_shared<Triangle>(triangle));
    }

    // 2. Utah Teapot
    // Load the OBJ file
    std::vector<point3D> teapot_vertices;
    std::vector<Triangle> teapot_faces;

    std::shared_ptr<Uniform_Hemispherical_Diffuse> red_material = std::make_shared<Uniform_Hemispherical_Diffuse>(Color(0.9, 0.3, 0.3));

    // Displacement vector
    Vec3D D = Vec3D(400.0, -0.5, 210.0);

    // Scale factor
    double scale_factor = 40.0;

    // Angle of rotation
    double angle_of_rotation = -59.5;

    // Material
    std::shared_ptr<Phong> royal_blue_phong = std::make_shared<Phong>(Color(0.25, 0.41, 1.0), 0.8, 2.5);
    std::shared_ptr<Material> glass = std::make_shared<Specular>(Color(1.0,1.0,1.0), 0.7, 1.0);

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

#endif //CUDA_RAY_TRACER_SCENES_H
