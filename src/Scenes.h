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

    return scene;
}


#endif //CUDA_RAY_TRACER_SCENES_H
