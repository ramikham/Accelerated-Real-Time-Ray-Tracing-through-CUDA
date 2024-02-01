//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SCENES_H
#define CUDA_RAY_TRACER_SCENES_H

#include "Utilities.h"
#include "Primitives/Primitives_Group.h"
#include "Primitives/Sphere.h"
#include "Primitives/Quad.h"
#include "Materials/Diffuse.h"
#include "Materials/Specular.h"
#include "Accelerators/BVH.h"
#include "Accelerators/Parallel_Bounding_Volume_Hierarchy.h"
#include "Primitives/Translate.h"
#include "Materials/Phong.h"
#include "Materials/Diffuse_Light.h"
#include "Mathematics/Transformations/Rotate_Y.h"
#include "Mathematics/Transformations/Rotate_Z.h"
#include "Mathematics/Transformations/Rotate_X.h"
#include "Primitives/Translate.h"
#include "Primitives/XY_Rectangle.h"
#include "Primitives/YZ_Rectangle.h"
#include "Primitives/XZ_Rectangle.h"

/*
Primitives_Group specular_scene(){
    Primitives_Group scene;

    // The ground material
    std::shared_ptr<Specular> specular_material_ground = std::make_shared<Specular>(Color(1.0, 1.0, 1.0), 0.0, 1.0);

    // The upper sphere material
    std::shared_ptr<Specular> specular_material_upper = std::make_shared<Specular>(Color(0.9, 0.3, 0.3), 0.5, 1.0);

    // Add to the scene
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -10000.5, -1.0), 10000.0, specular_material_ground));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, specular_material_upper));

    return scene;
}

Primitives_Group diffuse_ambient_scene(){
    Primitives_Group scene;

    // The ground material
    std::shared_ptr<Diffuse> diffuse_material_ground = std::make_shared<Diffuse>(Color(0.5, 0.5, 0.5));

    // The upper sphere material
    std::shared_ptr<Diffuse> diffuse_material_upper = std::make_shared<Diffuse>(Color(0.2, 0.3, 0.7));


    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, diffuse_material_ground));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, diffuse_material_upper));

    return scene;
}

Primitives_Group diffuse_ambient_scene_2(){
    Primitives_Group scene;

    // The ground material
    std::shared_ptr<Diffuse> diffuse_material_ground = std::make_shared<Diffuse>(Color(0.8, 0.8, 0.0));

    // The upper sphere material
    std::shared_ptr<Diffuse> diffuse_material_upper = std::make_shared<Diffuse>(Color(0.9, 0.3, 0.3));


    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, diffuse_material_ground));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, diffuse_material_upper));

    scene = Primitives_Group(std::make_shared<BVH>(scene));
    return scene;
}

Primitives_Group many_objects_scene() {
    Primitives_Group world;
    std::shared_ptr<Material> ground_material = std::make_shared<Diffuse>(Color(0.6, 0.6, 0.6));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0,-1000,0), 1000, ground_material));
    auto start = omp_get_wtime();
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3D center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3D(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;
                world.add_primitive_to_list(
                        std::make_shared<Sphere>(center, 0.2, std::make_shared<Diffuse>(Color(1.0, 1.0, 1.0))));
            }
        }
    }

    // Color big_sphere_color(0.8, 0.8, 0.8); // You can set the color you prefer
    //point3D big_sphere_center(0, 2.5, 0); // Adjust the center as needed
    //double big_sphere_radius = 2.0; // Adjust the radius as needed
    //std::shared_ptr<Material> sphere_material = std::make_shared<Diffuse>(Color(big_sphere_color));

    // std::shared_ptr<Specular> specular_material = std::make_shared<Specular>(Color(1.0, 1.0, 1.0), 0.0, 1.0);

    //world.add_primitive_to_list(std::make_shared<Sphere>(big_sphere_center, big_sphere_radius, specular_material));
    auto stop = omp_get_wtime();
    auto duration = stop - start;
    std::cout << "Allocation of spheres took " << duration << std::endl;

    start = omp_get_wtime();
    world = Primitives_Group(std::make_shared<BVH>(world));
    stop = omp_get_wtime();
    duration = stop - start;
    std::cout << "BVH BUILDING TOOK: " << duration << std::endl;
    return world;
}


// Final scene
Primitives_Group testing_scene() {
    Primitives_Group world;
    std::shared_ptr<Specular> ground_material = std::make_shared<Specular>(Color(1.0,1.0,1.0), 0.1, 1.0);

    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0,-3000,0), 3000, ground_material));

    std::shared_ptr<Diffuse> diffuse_material = std::make_shared<Diffuse>(Color(0.8, 0.8, 0.0));
    std::shared_ptr<Specular> specular_material = std::make_shared<Specular>(Color(0.9, 0.3, 0.3), 0.5, 1.0);

    // Create a stack of spheres
    int num_layers = 7;                     // number of layers in the stack
    int spheres_per_layer = 12;             // Number of spheres in each layer


    double pyramid_base_radius = 2.0;       // radius of the base layer
    double pyramid_height = 1.0;            // heaight of each layer

    for (int layer = 0; layer < num_layers; layer++) {
        double layer_height = layer * pyramid_height;

        for (int i = 0; i < spheres_per_layer; i++) {
            double angle = i * (360.0 / spheres_per_layer);
            double x = pyramid_base_radius * cos(degrees_to_radians(angle));
            double z = pyramid_base_radius * sin(degrees_to_radians(angle));
            double y = layer_height + pyramid_height / 2.0;

            // Different materials for alternating layers
            std::shared_ptr<Material> sphere_material;
            if (layer % 2 == 0) {
                sphere_material = diffuse_material;
            } else {
                sphere_material = specular_material;
            }

            world.add_primitive_to_list(std::make_shared<Sphere>(point3D(x-24, y, z-20), 0.5, sphere_material));
            world.add_primitive_to_list(std::make_shared<Sphere>(point3D(x-15, y, z-15), 0.5, sphere_material));
            world.add_primitive_to_list(std::make_shared<Sphere>(point3D(x-7, y, z-10), 0.5, sphere_material));
            world.add_primitive_to_list(std::make_shared<Sphere>(point3D(x, y, z-5), 0.5, sphere_material));
            world.add_primitive_to_list(std::make_shared<Sphere>(point3D(x+7, y, z+5), 0.5, sphere_material));
        }
    }

    // Construct a BVH
    //world = Primitives_Group(std::make_shared<BVH>(world));
    return world;
}
*/

/*
Primitives_Group translation_scene() {
    Primitives_Group world;
    Vec3D disp(0.0, 0.0, 0.0);
    std::shared_ptr<Material> ground_material = std::make_shared<Diffuse>(Color(0.6, 0.6, 0.6));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0, -1000, 0), 1000, ground_material, disp));

    // Number of spheres in each dimension (total 100 spheres)
    int spheres_per_dim = 5;

    // Spacing between spheres
    double spacing = 0.5;

    point3D center(-5 * spacing, 0.2, -5 * spacing);
    std::shared_ptr<Material> sphere_material = std::make_shared<Diffuse>(Color(1.0, 1.0, 1.0));
    auto first_instance =  std::make_shared<Sphere>(center, 0.2, sphere_material, disp+=Vec3D(0.1,0.0,0.0));
    world.add_primitive_to_list(first_instance);

    for (int a = -20; a < spheres_per_dim; a++) {
        for (int b = -20; b < spheres_per_dim; b++) {
            point3D center(a * spacing, 0.2, b * spacing);
            // You can adjust the material properties as needed
           // auto translated_sphere = std::make_shared<translate>(first_instance, Vec3D(a * spacing, 0.2, b * spacing));

            // Add the translated sphere to the world
           // world.add_primitive_to_list(translated_sphere);

         //  auto instance = std::make_shared<Sphere>(center, 0.2, sphere_material, disp);
          // world.add_primitive_to_list(instance);
        }
    }

   world = Primitives_Group(std::make_shared<BVH>(world));

    return world;
}*/

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

Scene_Information scene_Test() {
    // Image settings
    // -------------------------------------------------------------------------------
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 16.0 / 9.0;
    scene_info.image_width = 1200;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 50;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(13,2,3);
    scene_info.lookat = Vec3D(0, 0, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 20;
    scene_info.output_image_name = "ONB - BRDF Enabled - Diffuse";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    Primitives_Group world;

    /* DIFFUSE*/
    // Scene details
    // -------------------------------------------------------------------------------
    // The ground material
    std::shared_ptr<Diffuse> diffuse_material_ground = std::make_shared<Diffuse>(Color(0.8, 0.8, 0.0));

    // The upper sphere material
    std::shared_ptr<Diffuse> diffuse_material_upper = std::make_shared<Diffuse>(Color(0.9, 0.3, 0.3));

    world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, diffuse_material_ground));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, diffuse_material_upper));

    world = Primitives_Group(std::make_shared<BVH>(world, MIN_COORDINATE_SORT));

    scene_info.world = world;

    return scene_info;


    /*
    // Scene details
// -------------------------------------------------------------------------------
// The ground material
    std::shared_ptr<Phong> phong_material_ground = std::make_shared<Phong>(Color(0.8, 0.8, 0.0), 0.5, 32.0);

// The upper sphere material
    std::shared_ptr<Phong> phong_material_upper = std::make_shared<Phong>(Color(0.9, 0.3, 0.3), 0.5, 32.0);

    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0.0, -100.5, -1.0), 100.0, phong_material_ground));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0.0, 0.0, -1.0), 0.5, phong_material_upper));

    world = Primitives_Group(std::make_shared<BVH>(world, MIN_COORDINATE_SORT));

    scene_info.world = world;

    return scene_info;*/
}

Primitives_Group diffuse_ambient_scene_2(){
    Primitives_Group scene;

    // The ground material
    std::shared_ptr<Diffuse> diffuse_material_ground = std::make_shared<Diffuse>(Color(0.8, 0.8, 0.0));

    // The upper sphere material
    std::shared_ptr<Diffuse> diffuse_material_upper = std::make_shared<Diffuse>(Color(0.9, 0.3, 0.3));

    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, diffuse_material_ground));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, diffuse_material_upper));


    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            point3D center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - point3D(4, 0.2, 0)).length() > 0.9) {
                scene.add_primitive_to_list(
                        std::make_shared<Sphere>(center, 0.2, diffuse_material_upper));
            }
        }
    }

    scene = Primitives_Group(std::make_shared<BVH>(scene, MIN_COORDINATE_SORT));
    return scene;
}

/*
Primitives_Group many_objects_scene() {
    Primitives_Group world;
    Vec3D disp(0.0, 0.0, 0.0);
    Material* ground_material = new Diffuse(Color(0.6, 0.6, 0.6));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0,-1000,0), 1000, ground_material, disp));
    auto start = omp_get_wtime();

    for (int a = -41; a < 41; a++) {
        for (int b = -41; b < 41; b++) {
            point3D center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            //disp += Vec3D(0.1,0.1,0.1);
            if ((center - point3D(4, 0.2, 0)).length() > 0.9) {
                Material* sphere_material = new Diffuse(Color(1.0, 1.0, 1.0));
                world.add_primitive_to_list(
                        std::make_shared<Sphere>(center, 0.2, sphere_material, disp));
             //   delete sphere_material;
            }
        }
    }
    auto stop = omp_get_wtime();
    auto duration = stop - start;
    std::cout << "Allocation of spheres took " << duration << std::endl;

    start = omp_get_wtime();
    world = Primitives_Group(std::make_shared<BVH>(world));
    stop = omp_get_wtime();
    duration = stop - start;
    std::cout << "BVH BUILDING TOOK: " << duration << std::endl;
    return world;
}
 */

Scene_Information quads_scene() {
    // Image settings
    // -------------------------------------------------------------------------------
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 1.0;
    scene_info.image_width = 400;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 100;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(0,0,9);
    scene_info.lookat = Vec3D(0, 0, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 80;
    scene_info.output_image_name = "Quads World";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    auto left_red = std::make_shared<Diffuse>(Color(1.0, 0.2, 0.2));
    auto back_green = std::make_shared<Diffuse>(Color(0.2,1.0,0.2));
    auto right_blue = std::make_shared<Diffuse>(Color(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<Diffuse>(Color(1.0,0.5,0.0));
    auto lower_teal = std::make_shared<Diffuse>(Color(0.2,0.8,0.8));

    // Quads
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(-3,-2,5), Vec3D(0,0,-4), Vec3D(0,4,0), left_red));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(-2, -2, 0), Vec3D(4, 0, 0), Vec3D(0, 4, 0), back_green));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(3,-2,1), Vec3D(0,0,4), Vec3D(0, 4, 0), right_blue));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(-2,3,1), Vec3D(4,0,0), Vec3D(0,0,4), upper_orange));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(-2,-3,5), Vec3D(4,0,0), Vec3D(0,0,-4), lower_teal));

    return scene_info;
}

Scene_Information simple_light() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 16.0 / 9.0;
    scene_info.image_width = 1000;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 200;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(26,3,6);
    scene_info.lookat = Vec3D(0, 2, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 20;
    scene_info.output_image_name = "Light Test";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(1.0, 0.2, 0.2));
    Color light_intensity = Color(4,4,4);

    // Primitives
    // -------------------------------------------------------------------------------
/*    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(-2,-2,0),
                                                                  Vec3D(4,0,0),
                                                                  Vec3D(0,4,0),
                                                                  std::make_shared<Diffuse_Light>(light_intensity)));*/
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(3,1,-2),
                                                                  Vec3D(2,0,0),
                                                                  Vec3D(0,2,0),
                                                                  std::make_shared<Diffuse_Light>(light_intensity)));
    //scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(-2,2,-4), 2, red));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(0,2,0),2,red));

    return scene_info;
}

/// Reference: Ray Tracing: The Next Week
/// https://github.com/RayTracing/raytracing.github.io/blob/release/src/TheRestOfYourLife/quad.h
inline std::shared_ptr<Primitives_Group> Box(const point3D& a, const point3D& b, std::shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = std::make_shared<Primitives_Group>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = point3D(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    auto max = point3D(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    auto dx = Vec3D(max.x() - min.x(), 0, 0);
    auto dy = Vec3D(0, max.y() - min.y(), 0);
    auto dz = Vec3D(0, 0, max.z() - min.z());

    sides->add_primitive_to_list(std::make_shared<Quad>(point3D(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add_primitive_to_list(std::make_shared<Quad>(point3D(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add_primitive_to_list(std::make_shared<Quad>(point3D(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add_primitive_to_list(std::make_shared<Quad>(point3D(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add_primitive_to_list(std::make_shared<Quad>(point3D(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add_primitive_to_list(std::make_shared<Quad>(point3D(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

/// Reference: https://www.graphics.cornell.edu/online/box/data.html
Scene_Information Cornell_Box_scene() {
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
    scene_info.output_image_name = "Cornell Box - Mixture PDF";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));

    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(4,4,4));

    // Primitives
    // -------------------------------------------------------------------------------
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(555,0,0), Vec3D(0, 555, 0), Vec3D(0,0,555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(0,0,0), Vec3D(0,555,0), Vec3D(0,0,555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(343, 554, 332), Vec3D(-130,0,0), Vec3D(0,0,-105), light));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(0,0,0), Vec3D(555,0,0), Vec3D(0,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(555,555,555), Vec3D(-555,0,0), Vec3D(0,0,-555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(0,0,555), Vec3D(555,0,0), Vec3D(0,555,0), white));

    std::shared_ptr<Primitive> box1 = Box(point3D(0,0,0), point3D(165,330,165), white);
    std::shared_ptr<Primitive> box2 = Box(point3D(0,0,0), point3D(165,165,165), white);

    // Translation/Rotation
    // -------------------------------------------------------------------------------
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3D(265,0,295));
    scene_info.world.add_primitive_to_list(box1);

    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3D(130,0,65));
    scene_info.world.add_primitive_to_list(box2);

  //  scene_info.world = Primitives_Group(std::make_shared<BVH>(scene_info.world, MIN_COORDINATE_SORT));

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<Quad>(point3D(343, 554, 332), Vec3D(-130,0,0), Vec3D(0,0,-105), m));

    return scene_info;
}

/// Reference: https://www.graphics.cornell.edu/online/box/data.html
Scene_Information my_Cornell_Box_scene() {
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
    scene_info.output_image_name = "Cornell Box - Mixture PDF";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse> red = std::make_shared<Diffuse>(Color(0.65, 0.05, 0.05));
    std::shared_ptr<Diffuse> white = std::make_shared<Diffuse>(Color(0.73, 0.73, 0.73));
    std::shared_ptr<Diffuse> green = std::make_shared<Diffuse>(Color(0.12, 0.45, 0.15));

    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(4,4,4));


    std::shared_ptr<Diffuse_Light> sphere_light = std::make_shared<Diffuse_Light>(Color(0.2,0,0));
    // Primitives
    // -------------------------------------------------------------------------------
    // z's must be equal
 //   scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(343, 554, 332), Vec3D(-130,0,0), Vec3D(0,0,-105), light));

    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(555, 0, 0), point3D(555, 555, 555), green));
    scene_info.world.add_primitive_to_list(std::make_shared<YZ_Rectangle>(point3D(0,0,0), point3D(0, 555, 555), red));
    scene_info.world.add_primitive_to_list(std::make_shared<XY_Rectangle>(point3D(0, 0, 555), point3D(555, 555, 555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 0, 0), point3D(555,0,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(0, 555, 0), point3D(555,555,555), white));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D(190,90,190), 90, sphere_light));



 //   scene_info.world.add_primitive_to_list(std::make_shared<Quad>(point3D(343, 554, 332), Vec3D(-130,0,0), Vec3D(0,0,-105), light));

    std::shared_ptr<Primitive> box1 = Box(point3D(0,0,0), point3D(165,330,165), white);
    std::shared_ptr<Primitive> box2 = Box(point3D(0,0,0), point3D(165,165,165), white);

    // Translation/Rotation
    // -------------------------------------------------------------------------------
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3D(265,0,295));
    scene_info.world.add_primitive_to_list(box1);

    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3D(130,0,65));
  //  scene_info.world.add_primitive_to_list(box2);

  //  scene_info.world = Primitives_Group(std::make_shared<BVH>(scene_info.world, MIN_COORDINATE_SORT));

    // Lights
    // -------------------------------------------------------------------------------
    auto m = std::shared_ptr<Material>();
    //scene_info.lights.add_primitive_to_list(std::make_shared<Quad>(point3D(343, 554, 332), Vec3D(-130,0,0), Vec3D(0,0,-105), m));
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), m));
    scene_info.lights.add_primitive_to_list(std::make_shared<Sphere>(point3D(190,90,190), 90, m));

    return scene_info;
}

Scene_Information many_balls_scene() {
    Scene_Information scene_info;

    // Image settings
    // -------------------------------------------------------------------------------
    scene_info.aspect_ratio = 16.0 / 9.0;
    scene_info.image_width = 1200;
    scene_info.image_height = static_cast<int>(scene_info.image_width / scene_info.aspect_ratio);

    // Rendering settings
    // -------------------------------------------------------------------------------
    scene_info.max_depth = 10;
    scene_info.samples_per_pixel = 10;

    // Camera settings
    // -------------------------------------------------------------------------------
    scene_info.lookfrom = Vec3D(13, 2, 3);
    scene_info.lookat = Vec3D(0, 0, 0);
    scene_info.vup = Vec3D(0, 1, 0);
    scene_info.vfov = 20;
    scene_info.output_image_name = "Many Balls";

    scene_info.camera = Camera(scene_info.lookfrom, scene_info.lookat, scene_info.vup, scene_info.vfov, scene_info.aspect_ratio);

    // Materials
    // -------------------------------------------------------------------------------
    // The ground material
    std::shared_ptr<Diffuse> diffuse_material_ground = std::make_shared<Diffuse>(Color(0.8, 0.8, 0.0));

    // The upper sphere material
    std::shared_ptr<Diffuse> diffuse_material_upper = std::make_shared<Diffuse>(Color(0.9, 0.3, 0.3));

    // Primitives
    // -------------------------------------------------------------------------------
    std::shared_ptr<Diffuse_Light> light = std::make_shared<Diffuse_Light>(Color(20,20,20));
    scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(-10, 80, -10), point3D(0, 80, 0), light));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, diffuse_material_ground));
    scene_info.world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, diffuse_material_upper));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            point3D center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - point3D(4, 0.2, 0)).length() > 0.9) {
                scene_info.world.add_primitive_to_list(
                        std::make_shared<Sphere>(center, 0.2, diffuse_material_upper));
            }
        }
    }

   // scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(-10, 80, -10), point3D(0, 80, 0), light));


    // radiance_background: scene_info.world.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(213, 554, 227), point3D(343,554,332), light));
    auto m = std::shared_ptr<Material>();
    scene_info.lights.add_primitive_to_list(std::make_shared<XZ_Rectangle>(point3D(-10, 80, -10), point3D(0, 80, 0), m));
    scene_info.world = Primitives_Group(std::make_shared<BVH>(scene_info.world, MIN_COORDINATE_SORT));
    return scene_info;
}

#endif //CUDA_RAY_TRACER_SCENES_H
