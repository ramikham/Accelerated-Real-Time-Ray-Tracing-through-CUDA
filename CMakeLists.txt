cmake_minimum_required(VERSION 3.24)
project(CUDA_Ray_Tracer)

set(CMAKE_CXX_STANDARD 11)

add_executable(CUDA_Ray_Tracer src/main.cpp "src/Mathematics/Vec3D.h" "src/Utilities.h" "src/Mathematics/Ray.h" "src/Primitives/Primitive.h" "src/Camera.h" "src/Primitives/Sphere.h" "src/Primitives/Primitives_Group.h" "src/Mathematics/Probability/Randomized_Algorithms.h" "src/Scenes.h" "src/Scenes.h" "src/Shading.h" src/Materials/Material.h src/Materials/Diffuse.h src/Materials/Specular.h src/Accelerators/Axis_ALigned_Bounding_Box.h src/Accelerators/Axis_Aligned_Bounding_Box.h src/Accelerators/BVH.h src/Accelerators/Parallel_Bounding_Volume_Hierarchy.h src/translate.h src/Materials/Phong.h src/Materials/Uniform_Hemispherical_Diffuse.h src/onb.h src/Materials/Diffuse_Light.h src/Primitives/Quad.h src/Mathematics/Transformations/Rotate_Y.h src/Mathematics/Transformations/Rotate_Z.h src/Mathematics/Transformations/Rotate_X.h src/Primitives/Translate.h src/Mathematics/Probability/PDF.h src/Mathematics/Probability/Cosine_Weighted_PDF.h src/Mathematics/Probability/Uniform_Spherical_PDF.h src/Mathematics/Probability/Uniform_Hemispherical_PDF.h src/Mathematics/Probability/Primitive_PDF.h src/Mathematics/Probability/Mixture_PDF.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
