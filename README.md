# Accelerated-Real-Time-Ray-Tracing-through-CUDA

# Background

**Q. Why is this project important?**\
Ray tracing is the backbone of modern computer graphics and has become the most widely used rendering technique in the CG industry. There are many applications for ray tracing, including computer animation, video games, and architectural rendering. However, ray tracing does come with a downside: it is an unbounded algorithm that scales with the complexity of the scene. Using CUDA for ray tracing can significantly enhance the algorithm's performance. While, in theory, ray tracing converges to a realistic image given unlimited time, the value of time as a resource is paramount. Therefore, we aim to leverage the evolution of GPUs to transform ray tracing into a real-time algorithm, thereby expanding its usability across many more industries.

**Q. What has been done before?**


**Q. What gaps/limitations/challenges are there**


**Q. What method/tests/experiments will you do?**  
I will start with rendering scenes using the ray tracer I wrote in scratch using C++. I will then incorporate OpenMP and render the sames scenes. Finally, I will incorporate CUDA and render the same scenes. This will allow me to compare the performance of three versions of my ray tracers. The aim is to show that CUDA significantly beats any of the other versions. 

**Q. What data/tools/software/hardware will you need?**  
The technology that will be used is: C++, OpenMP, and CUDA.
The data that will be used are .obj files that will be rendered in the ray tracer: https://graphics.stanford.edu/data/3Dscanrep/ and https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html
The IDE that will be used is: CLion/Visual Studio

**Q. What results/data will you get/generate?**


**Q. How will you analyze/present/evaluate your data/results?**

I will **analyze** the results by comparing rendering times between three implementations:
  - sequential
  - Parallel on CPU (OpenMP)
  - Parallel on GPU (CUDA)
 
 Moreover, I will assess the visual quality of the rendered images by comparing them to published results.

 I will **present** the results using the following:
   - using *tables* to compare running times of the three implementations
   - using *graphs* to analyze the parallelism
   - rendering of images and animation videos
    

Project Timeline
Week 3
  - Work on completing the Ray Tracer program by implementing the following features:
    Camera
    Anti-aliasing
    Diffuse materials
    Metallic materials
    Dieelectric materials

    - Read the following research papers:
      Ray Tracing on Programmable Graphics Hardware

Week 4
  - Work on completing the Ray Tracer by implementing the following features
    Defocus Blur
    Motion Blur
    BVHs and AABBs
    Light

Week 5
 - Work on completing the Ray Tracer by implementing the following features
   Boxes
   Planes
   First scene of a Cornell Box

Week 6
  - Work on completing the Ray Tracer by adding the following features
    Triangles
    Cylinders

Week 7 
  - Work on completing the Ray Tracer by adding the following features
    Noise
    Meshes

Week 8 
  - Start on CUDA

References Used in Code
Reference       Class
[1]            Sphere.cpp
[2]            Primitives_Group.h
[3]            main.cpp

## Research Papers
The following is a table of all research papers read.
| Paper Title | Summary | 
| -------- | -------- | 
| *Ray Tracing on Prgrammable Graphics Hardware* | ~ |



## References

The following is a list of references, citations, or links that I consulted to write my code or to help clarify any misunderstood concepts.

 [1] *An Introduction to Ray Tracing - Section 2.1: Intersection of the Sphere – Algebraic Solution.*
 [2] *Fundamentals of Computer Graphics - section 4.4.4: Intersecting a Group of Objects.*
 [3] Writing a PPM file: https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++



