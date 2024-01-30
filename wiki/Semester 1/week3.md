# Week 3 Tasks

**25 September - 1 October**

- Meet supervisor:
- [x] Friday September, 29
  <br>
  <br>
- Implement the following features in the ray tracer:
- [x] Spheres 
- [x] Camera
- [x] Anti-aliasing (through multisampling)
- [x] Shadows
- [x] Shading Model
- [x] Diffuse Materials
- [x] Specular Materials
- [ ] Dielectric Materials
- [x] Test out simple ray tracer
  <br>
  <br>
- Reference books/journals/cites/papers that were used inc coding the ray tracer
- [x] *Introduction to Ray Tracing*
- [x] *Introduction to Ray Tracing*
- [x] *Introduction to Ray Tracing*
- [x] *Fundamentals of Computer Graphics*
  <br>
  <br>
- Update the research plan LaTeX document
- [x] Update *Appendix*

The following is a table of where the references were use in the code:

| Reference                                                                                               | Function/Class                                 | Purpose |
|---------------------------------------------------------------------------------------------------------|------------------------------------------------|--------|
| https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++                                            | rendering_loop_with_multisampling() - main.cpp | Writing to a PPM file |
| *Introduction to Ray Tracing: Chapter 2 - Section 2.1: Intersection of the Sphere – Algebraic Solution* | Sphere::intersection() - sphere.cpp            | Writing to a PPM file |
| *Introduction to Ray Tracing: Chapter 4 - Section 2.1: Intersection of the Sphere – Algebraic Solution* | rendering_loop_with_multisampling() - main.cpp | Writing to a PPM file |
| https://www.rosettacode.org/wiki/Bitmap/Write_a_PPM_file#C++                                            | rendering_loop_with_multisampling() - main.cpp | Writing to a PPM file |

## Results

Two diffuse spheres rendered with 50 samples-per-pixel.
![Test-Diffuse-50-Samples-per-Pixel.png](..%2FImages%2FTest-Diffuse-50-Samples-per-Pixel.png)

The same scene with 500 samples-per-pixel.
![Test-Diffuse-500-Samples-per-Pixel.png](..%2FImages%2FTest-Diffuse-500-Samples-per-Pixel.png)

Two diffuse spheres and two specular spheres rendered with 200 samples-per-pixel.
![Test-Specular-200-Samples-per-Pixel.png](..%2FImages%2FTest-Specular-200-Samples-per-Pixel.png)
---

## Tasks Not Completed
- Implementing dielectric materials
---
## Summary
In this week, I started writing code for the implementation of my renderer. I will use C++ for now. In the future, I am planning to use CUDA.
