# Approaches to Accelerating Rendering Speeds
Computational demands of rendering can be significant, especially for complex scenes with high-resolution
geometric data and intricate lighting. This necessitates the development of efficient techniques to accelerate rendering speeds. In what follows, we will explore the theoretical foundations of rendering and various
acceleration techniques. We will cover topics including rendering algorithms, shading calculations, acceleration data structure, optimizing ray/object intersection tests, and the impact of parallelism on rendering
times. We will implement a rendering engine and investigate methods to improve its performance. We
conclude by analyzing how our acceleration techniques improve the speed of our rendering engine
## Background
Some of the rendered images can be found [here](Images).<br>


## Gallery

This scene was rendered with 2,000 samples-per-pixel. All the materials in this scene are diffuse.</br></br>
![Rabbit](Images/Semester%202/a_rabbit_and_a_teapot_inside_a_Cornell_box_2000SPPP.png)</br></br>
This scene was rendered with 3,000 samples-per-pixel. All the materials in this scene are Phong. 
Because Phong materials have a reflective component, we notice the presence of some white sprinkles scattered 
around the scene. These are not to be confused with shadow acne (which are caused by numerical precision errors); 
they are reflections of the white light in the ceiling.</br></br>
![Phong](Images/Semester%202/Enter-the-Dragon-3000SPP.jpg)</br></br>
This scene was rendered with 5,000 samples-per-pixel.</br></br>
![Lucy Mirror](Images/Semester%202/Lucy-with-a-Mirror-5000-SPP.jpg)
Another Lucy scene. This time with Utah's Teapot.</br></br>
![Lucy and Teapot](Images/Semester%202/Lucy_and_Utah_Teapot.jpg)

## References
The 3D models used in this project were taken from the following websites:
- https://graphics.stanford.edu/data/3Dscanrep/
- https://graphics.cs.utah.edu/teapot/
- https://www.graphics.cornell.edu/online/box/

