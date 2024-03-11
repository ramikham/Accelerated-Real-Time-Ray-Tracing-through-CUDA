#include "Rendering/Serial_Rendering_Functions.h"
#include "Rendering/Parallel_Rendering_Functions.h"
#include "Unit Testing/Functions_Tests.h"

int main() {

        auto start = omp_get_wtime();

        Scene_Information scene_info = enter_Lucy(); //bunny_test();

        // parallel_loop_radiance_background_renderer(scene_info);
        // parallel_tasks_radiance_background_renderer(scene_info);
        // parallel_cols_workload_radiance_background(scene_info);

        parallel_loop_radiance_mixture_renderer(scene_info);
        // parallel_tasks_radiance_mixture_renderer(scene_info);       // my to-go function
        // parallel_cols_workload_radiance_mixture_renderer(scene_info);

        auto stop = omp_get_wtime();

        auto duration = stop - start;

        std::cout << duration << std::endl;

    return 0;
}

