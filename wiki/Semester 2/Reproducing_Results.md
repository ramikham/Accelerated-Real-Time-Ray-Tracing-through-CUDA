# How to Reproduce Results

We have shown several results in our dissertation. For the sake of reproducing them,
we will show a step-by-step guid.

___
## Experiment: Comparing Ray/AABB Intersection Algorithms

|                                        |                                                                    |
|----------------------------------------|--------------------------------------------------------------------|
| **Where it is used**                   | Section 7.2.3                                                      |
| **Rendering Function**                 | `parallel_loop_radiance_mixture_renderer()`                        |
| **Scene**                              | `a_rabbit_and_a_teapot_inside_a_Cornell_box_1()`                   |
| **Samples-per-pixel**                  | [2, 4, 8, 16, 32, 64]                                              |
| **Max Depth**                          | 10                                                                 |
| **Threads**                            | 16                                                                 |
| **Ray/Triangle Intersection Algorithm** | `Moller_Trumbore_ray_triangle_intersection()`                      |
| **BVH Used**                           | `BVH_Fast<>` - rotational axes / compare by centroid / nth_element |
| **Variable of the Experiment**         | Ray/AABB Intersection Algorithm                                    |

![rabbit](../../Images/Semester%202/a_rabbit_and_a_teapot_inside_a_Cornell_box_2000SPPP.png)

___
Experiment: Comparing BVH Implementations.

Note: Calls were repeated for 10 times and the median was taken.

|                                         |                                                   |
|-----------------------------------------|---------------------------------------------------|
| **Where it is used**                    | Section 7.2.3                                     |
| **Rendering Function**                  | `parallel_loop_radiance_mixture_renderer()`       |
| **Scene**                               | `a_rabbit_and_a_teapot_inside_a_Cornell_box_1()`  |
| **Samples-per-pixel**                   | 10                                                |
| **Max Depth**                           | 10                                                |
| **Threads**                             | 16                                                |
| **Ray/Triangle Intersection Algorithm** | `Moller_Trumbore_ray_triangle_intersection()`     |
| **Ray/AABB Intersection Algorithm**     | `Williams_ray_AABB_intersection()`                |
| **Variable of the Experiment**          | The BVH used                                      |

![rabbit](../../Images/Semester%202/a_rabbit_and_a_teapot_inside_a_Cornell_box_2000SPPP.png)

## Experiment: Comparing BVH Implementations.

Note: Calls were repeated for 10 times and the median was taken.

|                                         |                                                |
|-----------------------------------------|------------------------------------------------|
| **Where it is used**                    | Section 7.2.3                                  |
| **Rendering Function**                  | `parallel_loop_radiance_mixture_renderer()`    |
| **Scene**                               | `enter_lucy()`                                 |
| **Samples-per-pixel**                   | 10                                             |
| **Max Depth**                           | 10                                             |
| **Threads**                             | 16                                             |
| **Ray/Triangle Intersection Algorithm** | `Moller_Trumbore_ray_triangle_intersection()`  |
| **Ray/AABB Intersection Algorithm**     | `Williams_ray_AABB_intersection()`             |
| **Variable of the Experiment**          | The BVH used: `BVH_Fast<>` vs `BVH`            |


![lucy](../../Images/Semester%202/Lucy_and_Utah_Teapot.jpg)

___
## Experiment: Comparing our Three Parallel Strategies Implementations

|                                          |                                                                    |
|------------------------------------------|--------------------------------------------------------------------|
| **Where it is used**                     | Section 7.4.1                                                      |
| **Scene**                                | `lucy_with_a_mirror()`                                             |
| **Samples-per-pixel**                    | 10                                                                 |
| **Max Depth**                            | 10                                                                 |
| **Ray/Triangle Intersection Algorithm**  | `Moller_Trumbore_ray_triangle_intersection()`                      |
| **BVH Used**                             | `BVH_Fast<>` - rotational axes / compare by centroid / nth_element |
| **Variable of the Experiment**           | Rendering Function & Number of threads                             |

![lucy_mirror](../../Images/Semester%202/Lucy-with-a-Mirror-5000-SPP.jpg)


## Experiment: Comparing our Parallel Strategies Implementations to [He et al., 2021]
|                                       |                                                                                                  |
|---------------------------------------|--------------------------------------------------------------------------------------------------|
| **Where it is used**                  | Section 7.4.1                                                                                    |
| **Rendering Function**                | `parallel_cols_workload_radiance_background()` & `parallel_tasks_radiance_background_renderer()` |
| **Scene**                             | `one_weekend_scene()`                                                                            |
| **Samples-per-pixel**                 | 100                                                                                              |
| **Max Depth**                         | 5                                                                                                |
| **Ray/Sphere Intersection Algorithm** | `ray_sphere_intersection_algebraic_solution()`                                                   |
| **BVH Used**                          | `BVH_Fast<>` - rotational axes / compare by centroid / nth_element                               |
| **Variable of the Experiment**        | Number of threads                                                                                |

![lucy_mirror](../../Images/Semester%202/One_Weekend_Scene.png)