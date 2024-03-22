As I mentioned in my dissertation, I took some inspiration from the Ray Tracing in One
Weekend book series in implementing my engine. Nonetheless, I made many changes
and added many features. I will list the major ones below:
1. A 2D Vector Class.
    - I implemented a new `Vec2D` class. It was needed for implementing triangles.

2. CPU Multithreading.
    - I used OpenMP to implement three strategies to parallelize my engine:
        1. *Parallel loop* strategy.
        2. *Parallel columns* strategy.
        3. *Parallel tasks* strategy.
    - These have been discussed throughout the text, and we have seen that they greatly enhanced the speed of our engine.

3. GPU Multithreading.

4. BVH Implementation. I used different heuristics to construct the BVH, and I implemented an optimized version.

5. New materials.
    - Shirley's code has three material types: Lambertian (i.e., perfect diffuse), metal, and dielectric. My engine expands these materials and implements new ones:
        1. The perfect diffuse is rewritten more efficiently and is found in the `Diffuse` class.
        2. Other diffuse materials, like `Uniform_Hemispherical_Diffuse`.
        3. A `Specular` class that combines both metal and dielectric.
        4. A `Phong` class for Phong material.

6. Importance Sampling.
    1. Phong. See Figure \ref{fig:Phong_Importance_Sampling}.

7. New ray/sphere intersection test.
    - I implemented the *geometric solution* for testing ray/sphere intersections. I was hoping that it would be faster than the *algebraic solution*.

8. New ray/AABB intersection tests.
    - Shirley's code uses the *Slab method*; however, as we have seen this method is very inefficient. Therefore, I implemented several other ray/AABB intersection algorithms to accelerate my engine.
        1. *Branchless method*.
        2. *Kensler Method*.
        3. *Optimized slab method*.
        4. A new method that I wrote.

9. New *Triangle* class.
    - Shirley's code doesn't have a triangle class; however, to render meshes, I needed to implement one.
    - To display triangles, I implemented two ray/triangle intersection algorithms:
        1. The *Möller–Trumbore* ray-triangle intersection algorithm.
        2. The *Snyder & Barr* ray-triangle intersection algorithm.
    - I implemented a method to sample points on the triangle. This was needed to render less-noisy triangles when light is used.
    - Accordingly, I implemented a method to calculate the PDF value of sampling a random direction on the triangle.
    - I implemented a method to allow constructing an AABB for a triangle, and thus render them faster using the BVH class.

10. Meshes.
    - Shirley's code has no meshes. Using the *Triangle* class that I implemented, I was able to read .obj files and build meshes using triangles in the process. This allowed me to render all the meshes that were shown in the research. Loading the meshes also allowed the user to rotate, scale, or translate the object however they wish.

11. Different way of calculating shading.

12. New Transformations. This includes rotation over different axes, scaling, translation, etc...
