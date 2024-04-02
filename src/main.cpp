#include "Rendering/Serial_Rendering_Functions.h"
#include "Rendering/Parallel_Rendering_Functions.h"
#include "Unit Testing/Functions_Tests.h"

int main() {

        auto start = omp_get_wtime();

        // Choose one of the scenes to render
        // -----------------------------------------------------------------------
        // Scene_Information scene_info = one_weekend_scene();
        // Scene_Information scene_info = enter_the_dragon();
        // Scene_Information scene_info = Lucy_with_light();
        // Scene_Information scene_info = enter_Lucy();
        // Scene_Information scene_info = Lucy_with_a_mirror();
        // Scene_Information scene_info = a_rabbit_and_a_teapot_inside_a_Cornell_box();
        // Scene_Information scene_info = a_rabbit_and_a_teapot_inside_a_Cornell_box_without_importance_sampling();
        // Scene_Information scene_info = full_Cornell_box();
        Scene_Information scene_info = texture_Cornell_box();
        // Scene_Information scene_info = different_diffuse_models_scene();

        // Serial rendering function
        // -----------------------------------------------------------------------
        // serial_radiance_renderer(scene_info);

        // Parallel rendering functions without importance sampling
        // -----------------------------------------------------------------------
        // parallel_loop_radiance_background_renderer(scene_info);
        // parallel_tasks_radiance_background_renderer(scene_info);
        // parallel_cols_workload_radiance_background(scene_info);

        // Parallel rendering functions with importance sampling
        // -----------------------------------------------------------------------
        // parallel_loop_radiance_mixture_renderer(scene_info);
        parallel_tasks_radiance_mixture_renderer(scene_info);                   // my to-go function
        // parallel_cols_workload_radiance_mixture_renderer(scene_info);

        auto stop = omp_get_wtime();
        auto duration = stop - start;
        std::cout << duration << std::endl;

    return 0;
}

