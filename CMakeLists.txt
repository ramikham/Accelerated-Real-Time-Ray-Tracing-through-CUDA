cmake_minimum_required(VERSION 3.24)
project(CUDA_Ray_Tracer)

set(CMAKE_CXX_STANDARD 11)

add_executable(CUDA_Ray_Tracer main.cpp Mathematics/Vec3D.h Utilities.h Mathematics/Ray.cpp Mathematics/Ray.h Primitives/Primitive.h Camera.h Primitives/Sphere.cpp Primitives/Sphere.h Primitives/Primitives_Group.cpp Primitives/Primitives_Group.h Mathematics/Randomized_Algorithms.h Scenes.h Scenes.h Shading.h)
