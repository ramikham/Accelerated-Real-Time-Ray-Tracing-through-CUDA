cmake_minimum_required(VERSION 3.24)
project(CUDA_Ray_Tracer)

set(CMAKE_CXX_STANDARD 11)

add_executable(CUDA_Ray_Tracer src/main.cpp "src/Mathematics/Vec3D.h" "src/Utilities.h" "src/Mathematics/Ray.h" "src/Primitives/Primitive.h" "src/Cameras/Camera.h" "src/Primitives/Sphere.h" "src/Primitives/Primitives_Group.h" "src/Mathematics/Probability/Randomized_Algorithms.h" "src/Scenes.h" "src/Scenes.h" "src/Shading.h" src/Materials/Material.h src/Materials/Diffuse.h src/Materials/Specular.h src/Accelerators/AABB.h src/Accelerators/AABB.h src/Accelerators/BVH.h src/Materials/Phong.h src/Materials/Uniform_Hemispherical_Diffuse.h src/Materials/Diffuse_Light.h src/Mathematics/Transformations/Rotate_Y.h src/Mathematics/Transformations/Rotate_Z.h src/Mathematics/Transformations/Rotate_X.h src/Mathematics/Transformations/Translate.h src/Mathematics/Probability/PDF.h src/Mathematics/Probability/Cosine_Weighted_PDF.h src/Mathematics/Probability/Uniform_Spherical_PDF.h src/Mathematics/Probability/Primitive_PDF.h src/Mathematics/Probability/Mixture_PDF.h src/Primitives/XY_Rectangle.h src/Primitives/XZ_Rectangle.h src/Primitives/YZ_Rectangle.h src/Mathematics/Probability/Uniform_Hemispherical_PDF.h src/Primitives/Triangle.h src/Cameras/Orthographic_Camera.h src/Rendering/Parallel_Rendering_Functions.h src/Rendering/Serial_Rendering_Functions.h "src/Unit Testing/Functions_Tests.h" src/Mathematics/Vec2D.h src/Accelerators/BVH_Max_Coordinate.h src/Accelerators/BVH_Centroid_Coordinate.h src/Mathematics/Probability/Specular_PDF.h src/Accelerators/BVH_Fast.h src/Primitives/Box.h src/Accelerators/BVH_Parallel.h src/Textures/Texture.h src/Materials/Diffuse_With_Texture.h src/Textures/Perlin_Noise/Perlin.h src/Materials/Disney_Diffuse.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -fno-finite-math-only")

# More info to try later: https://stackoverflow.com/questions/3005564/gcc-recommendations-and-options-for-fastest-code
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
