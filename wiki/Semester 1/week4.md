# Week 4 Tasks

**2 October - 8 October**
- #### Meet supervisor
- [x] Friday October, 6
  <br>
  <br>
- #### Implement the following features in the ray tracer:
- [x] Refactor the code
    - The shading model was dumped inside the *cst_ray()* function, while this was ok for a
      first rendering image, I think it would be wise to refactor this shading formulation into
      several classes that inherit from one class (Material.h). This will make it easier for me to define
      the scattering formulation of the different materials. This will mean that there is more separation
      in my code.
      This refactoring will also mean that I have to cheange my Sphere.h class.
- [x] Update the Camera class to allow the camera to be orientable and positionable
- [x] Update the Camera class to enable "Defocus Blur"
- [ ] Bounding Volume Hierarchies
- [ ] Axis Aligned Bounding Boxes
- [ ] Shading Model
- [ ] Light
  <br>
  <br>
- #### Test out the renderer
- [ ] Compare the speed improvement achieved to last week's version after implementing BVHs and AABBs
- [ ] Compare the visual/render quality of images rendered with different samples-per-pixel count
- [ ] Compare the visual/render quality of images rendered with different maximum recursion depth
- [ ] Compare the visual/render quality of images rendered with different shading models
- [ ] Compare the visual/render quality of images rendered with lights vs ambient light only
  <br>
  <br>
- #### Start the *Literature Review*
- [x] Included 3 research papers in the LaTeX document

| Research Paper                                                       | Section in LaTeX Document             | Purpose                                                        | Citation (Harvard)                                                                                                                                                                                     |
|----------------------------------------------------------------------|---------------------------------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **Some Techniques for Shading Machine Renderings of Solids**         | **2.2.2 Ray Casting and Ray Tracing** | History of ray casting                                         | Appel, A., 1968, April. Some techniques for shading machine renderings of solids. In *Proceedings of the April 30--May 2, 1968, spring joint computer conference* (pp. 37-45).                         |
| **An Improved Illumination Model for Shaded Display**                | **2.2.2 Ray Casting and Ray Tracing** | History of ray recursive ray tracing                           | Whitted, T., 1979, August. An improved evaluate model for shaded display. In *Proceedings of the 6th annual conference on Computer graphics and interactive techniques* (p. 14).                   |
| **Models of light reflection for computer synthesized pictures**     | **2.3 Shading**                       | History of the Blinn-Phong shading model                       | Blinn, J.F., 1977, July. Models of light reflection for computer synthesized pictures. In *Proceedings of the 4th annual conference on Computer graphics and interactive techniques* (pp. 192-198).    |
| **Software Rasterization of 2 Billion Points in Real Time**          | **2.2.1 Rasterization**               | Recent breakthrough in real-time rendering using rasterization | Schütz, M., Kerbl, B. and Wimmer, M., 2022. Software rasterization of 2 billion points in real time. *Proceedings of the ACM on Computer Graphics and Interactive Techniques*, 5(3), pp.1-17.          |
| **Durer, Drawwing, and Digital Thinking - Steve Luecking**           | **2.2.2 Ray Casting and Ray Tracing** | History of ray tracing                                         | www.brian-curtis.com. (n.d.). *Durer, Drawwing, and Digital Thinking - Steve Luecking*. [online] Available at: https://www.brian-curtis.com/text/conferpape_steveluecking.html [Accessed 9 Oct. 2023]. |

- #### Update *Appendix*
- [x] Included more references used in the code
- #### Write the *Abstract* section
- [x] First draft
- #### Write in the *Introduction* section
- [x] First Draft
---
## Tasks Not Completed
1. Implementing the following features: <br>
&ensp;- Bounding Volume Hierarchies
   &ensp;- Axis Aligned Bounding Boxes
   &ensp;- Shading Model &ensp;- Light

2. Testing out the renderer with the new features

---
## Summary
In this week, I focused on my writing my LaTeX document. I have started my research review.