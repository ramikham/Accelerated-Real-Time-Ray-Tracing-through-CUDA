# Week 3 Tasks

**25 September - 1 October**

- [x] Meet supervisor
- [x] Review literature
- - [x] *Improving the Performance of a Ray Tracing Algorithm using a GPU*
- - [x] *Ray Tracing on Programmable Graphics Hardware*
- [ ] Implement the following features in the ray tracer:
- - [x] Spheres 
- - [x] Camera
- - [x] Anti-aliasing (through multisampling)
- - [x] Shadows
- - [x] Shading Model
- - [x] Diffuse Materials
- - [x] Specular Materials
- - [ ] Dielectric Materials
- [x] Test out simple ray tracer
- [x] Reference books/journals/cites/papers that were used inc coding the ray tracer
- - [x] *Introduction to Ray Tracing*
- - [x] *Introduction to Ray Tracing*
- - [x] *Introduction to Ray Tracing*
- - [x] *Fundamentals of Computer Graphics*
- - [x] 
- [x] Update the research plan LaTeX document
- - [x] Update *Appendix*

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



# Week 4 Tasks

**22 October - 8 October**
- [ ] Meet supervisor
- [x] Review literature
- - [x] **
- - [x] **

Implement the following features in the ray tracer:
- [ ] Refactor the code
    - The shading model was dumped inside the *cst_ray()* function, while this was ok for a
  first rendering image, I think it would be wise to refactor this shading formulation into
  several classes that inherit from one class (Material.h). This will make it easier for me to define 
  the scattering formulation of the different materials. This will mean that there is more separation 
  in my code.
  This refactoring will also mean that I have to cheange my Sphere.h class.
- [ ] Update the Camera class to allow the camera to be orientable and positionable
- [ ] Update the Camera class to enable "Defocus Blur" 
- [ ] Bounding Volume Hierarchies
- [ ] Axis Aligned Bounding Boxes
- [ ] Shading Model
- [ ] Light

Test out the renderer
- - [ ] Compare the speed improvement achieved to last week's version after implementing BVHs and AABBs
- - [ ] Compare the visual/render quality of images rendered with different samples-per-pixel count
- - [ ] Compare the visual/render quality of images rendered with different maximum recursion depth
- - [ ] Compare the visual/render quality of images rendered with different shading models
- - [ ] Compare the visual/render quality of images rendered with lights vs ambient light only
- [ ] Update the research plan LaTeX document
- - [ ] Update *Appendix*
- - [x] First draft of the *Abstract* 
- - [] First draft of the *Introduction*

---

## Notes
