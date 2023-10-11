cmake_minimum_required(VERSION 3.24)
project(CUDA_Ray_Tracer)

set(CMAKE_CXX_STANDARD 11)

add_executable(CUDA_Ray_Tracer src/main.cpp "src/Mathematics/Vec3D.h" "src/Utilities.h" "src/Mathematics/Ray.cpp" "src/Mathematics/Ray.h" "src/Primitives/Primitive.h" "src/Camera.h" "src/Primitives/Sphere.cpp" "src/Primitives/Sphere.h" "src/Primitives/Primitives_Group.cpp" "src/Primitives/Primitives_Group.h" "src/Mathematics/Randomized_Algorithms.h" "src/Scenes.h" "src/Scenes.h" "src/Shading.h" src/Materials/Material.h src/Materials/Diffuse.h src/Materials/Specular.h)
