//
// Created by Rami on 9/29/2023.
//

#ifndef CUDA_RAY_TRACER_SCENES_H
#define CUDA_RAY_TRACER_SCENES_H

# include "Utilities.h"
#include "Primitives/Primitives_Group.h"
#include "Primitives/Sphere.h"

Primitives_Group first_scene(){
    Primitives_Group scene;

    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0, -100.5, -1.0), 100.0, DIFFUSE, Color(0.5, 0.5, 0.5)));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 0.0,    0.0, -1.0),   0.5, DIFFUSE, Color(0.2, 0.3, 0.7)));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D(-1.0,    0.0, -1.0),   0.5, SPECULAR, Color(0.8, 0.8, 0.8)));
    scene.add_primitive_to_list(std::make_shared<Sphere>(point3D( 1.0,    0.0, -1.0),   0.5, SPECULAR, Color(0.8, 0.8, 0.8)));

    return scene;
}


#endif //CUDA_RAY_TRACER_SCENES_H
