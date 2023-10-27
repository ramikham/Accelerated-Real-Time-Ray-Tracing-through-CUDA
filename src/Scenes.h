//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SCENES_H
#define CUDA_RAY_TRACER_SCENES_H

# include "Utilities.h"
#include "Primitives/Primitives_Group.h"
#include "Primitives/Sphere.h"
#include "Materials/Diffuse.h"
#include "Materials/Specular.h"
#include "Accelerators/Bounding_Volume_Hierarchy.h"

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

    scene = Primitives_Group(std::make_shared<Bounding_Volume_Hierarchy>(scene));
    return scene;
}

Primitives_Group many_objects_scene() {
    Primitives_Group world;

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            point3D center(a + 0.9*random_double(), 1, b + 0.9*random_double());

            if ((center - point3D(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                    // diffuse
                    sphere_material = std::make_shared<Diffuse>(Color(0.4, 0.2, 0.1));
                    world.add_primitive_to_list(std::make_shared<Sphere>(center, 0.2, sphere_material));
            }
        }
    }

    world = Primitives_Group(std::make_shared<Bounding_Volume_Hierarchy>(world));
    return world;
}

Primitives_Group testing_scene() {
    Primitives_Group world;
    std::shared_ptr<Specular> specular_material_upper = std::make_shared<Specular>(Color(0.9, 0.3, 0.3), 0.5, 1.0);


    auto material_ground = std::make_shared<Diffuse>(Color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Diffuse>(Color(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<Specular>(Color(0.8, 0.6, 0.2), 0.5, 1.0);
    auto material_right  = std::make_shared<Specular>(Color(0.8, 0.6, 0.2), 0.0, 1.0);

    world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add_primitive_to_list(std::make_shared<Sphere>(point3D( 1.0,    0.0, -1.0),   0.5, material_right));

    return world;
}
#endif //CUDA_RAY_TRACER_SCENES_H
