#include "Rendering/Serial_Rendering_Functions.h"
#include "Rendering/Parallel_Rendering_Functions.h"
#include "Unit Testing/Functions_Tests.h"

int main() {


    auto start = omp_get_wtime();

    Scene_Information scene_info = a_rabbit_and_a_teapot_inside_a_Cornell_box_1(); //bunny_test();

   parallel_loop_radiance_mixture_renderer(scene_info);

    auto stop = omp_get_wtime();

    auto duration = stop - start;

    std::cout << duration << std::endl;

    scene_info.rendering_time = duration;

    std::cout << "BOX INTERSECTION CALLED = " << num_calls_box_intersection << std::endl;
    std::cout << "TRIANGLE INTERSECTION CALLED = " << num_calls_triangle_intersection << std::endl;
    std::cout << "RANDOM_DOUBLE() CALLED = " << num_calls_rand_double << std::endl;


  //  BENCHMARK::compare_uniform_random_double_generators();
  //  BENCHMARK::compare_ray_triangle_intersection_algorithms();

    //  std::cout << 800/8+1;

    //test_AABB_intersection();

    return 0;
}

