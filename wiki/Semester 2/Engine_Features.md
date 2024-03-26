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
   - I used CUDA to port the CPU implementation of my engine to the GPU ([GPU version](https://github.com/ramikham/Approaches-to-Accelerating-Rendering-Speeds-GPU-Version)).

4. BVH Implementation.
   - I used different heuristics to construct the BVH.
   - Peter Shirley's code uses `std::sort` to group the objects by the minimum coordinates of 
   their bounding boxes along the partition axis, which is chosen at random.
   - I have experimented using other sorting functions (e.g., `std::nth_element()`), and found that they can 
   accelerate rendering.
   - In my implementation, I extended this BVH implementation to allow sorting objects by the maximum coordinates and 
   centroids of their bounding boxes, which proved to yield an appreciable speedup.
   - Moreover, instead of choosing the split axis at random, I allow for choosing axis rotationally (first X, then Y, then Z).
   This has also shown to accelerate rendering.
   - Finally, I optimized the implementation of the BVH by changing the way the lists of objects are passed
   through each recursive calls, which significantly decreases the build time of the hierarchy and increases rendering speeds.

5. New materials.
    - Shirley's code has three material types: Lambertian (i.e., perfect diffuse), metal, and dielectric. My engine expands these materials and implements new ones:
        1. The perfect diffuse is rewritten more efficiently and is found in the `Diffuse` class.
        2. Other diffuse materials, like `Uniform_Hemispherical_Diffuse`.
        3. A `Specular` class that combines both metal and dielectric.
        4. A `Phong` class for Phong material.

6. Importance Sampling.
   - Phong: I implemented a version of the `Phong` class that has incorporated importance sampling.
       The following ([research paper](https://www.cs.princeton.edu/courses/archive/fall16/cos526/papers/importance.pdf)) helped me to achieve this.

7. New ray/sphere intersection test.
    - I implemented the *geometric solution* for testing ray/sphere intersections. I was hoping that it would be faster than the *algebraic solution*.

8. New ray/AABB intersection tests.
    - Shirley's code uses the *slab method*; however, as we have seen this method is very inefficient. Therefore, I implemented several other ray/AABB intersection algorithms to accelerate my engine.
        1. *Branchless method* ([link](https://tavianator.com/2022/ray_box_boundary.html)).
        2. *Kensler's Method* ([link](https://psgraphics.blogspot.com/2016/02/new-simple-ray-box-test-from-andrew.html)).
        3. *Optimized slab method* ([link](https://people.csail.mit.edu/amy/papers/box-jgt.pdf)).
        4. A new method that I wrote.
    - All these methods proved to be faster than the *slab method*.

9. New *Triangle* class.
    - Shirley's code doesn't have a triangle class; however, to render meshes, I needed to implement one.
    - To display triangles, I implemented two ray/triangle intersection algorithms:
        1. The *Möller–Trumbore* ray-triangle intersection algorithm ([link](https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf)).
        2. The *Snyder & Barr* ray-triangle intersection algorithm ([link](https://www.microsoft.com/en-us/research/wp-content/uploads/2017/01/p119-snyder.pdf)).
    - I implemented a method to sample points on the triangle. This was needed to render less-noisy triangles when light is used.
    - Accordingly, I implemented a method to calculate the PDF value of sampling a random direction on the triangle.
    - I implemented a method to allow constructing an AABB for a triangle, and thus render them faster using the BVH class.
    - I implemented support functions to read .obj files and facilitate meshes loading in order to render them. The functions allow
    the programmer to scale, rotate, and translate the loaded mesh.
10. Meshes.
    - Shirley's code has no meshes. Using the *Triangle* class that I implemented, I was able to read .obj files and build meshes using triangles in the process. This allowed me to render all the meshes that were shown in the research. Loading the meshes also allowed the user to rotate, scale, or translate the object however they wish.

11. Different way of calculating shading.
    - When calculating shading, I explicitly calculate the BRDF. I have found this helpful in following the research papers.


12. New Transformations. 
    - Shirley's code implements two classes: `Translate` and `Rotate_Y`, where the latter rotates objects along the Y-axis.
    - I implemented other classes to allow for rotations along the X and Z axis.
