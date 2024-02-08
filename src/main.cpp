#include "Rendering/Serial_Rendering_Functions.h"
#include "Rendering/Parallel_Rendering_Functions.h"
#include "Unit Testing/Functions_Tests.h"

int main() {
    auto start = omp_get_wtime();

 //   Scene_Information scene_info = scene_Test();
    Scene_Information scene_info = bunny_test(); //bunny_test();

  //  parallel_render_radiance_mixture(scene_info);      //parallel
    //serial_render(scene_info);



    parallel_cols_workload_radiance_background(scene_info);

    auto stop = omp_get_wtime();

    auto duration = stop - start;

    std::cout << duration << std::endl;

    scene_info.rendering_time = duration;

    //test_sphere_intersection();
    //test_triangle_intersection();


    return 0;
}
