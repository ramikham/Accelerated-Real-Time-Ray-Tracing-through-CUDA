# Title: Accelerated-Real-Time-Ray-Tracing-through-CUDA
Here we combine GPU programming through CUDA, alongside other accelerating schemes, to determine whether an offline rendering engine can be made to run in real-time.
We delve into the theoretical foundations of real-time rendering, discuss the computational demands 
dictated by realizing photo-realistic rendering in real-time, propose acceleration techniques by utilizing the parallelism 
of CUDA to harness the power of the graphics processing unit (GPU), discuss the limitations that have restricted the usage of real-time rendering in highly 
demanding computing applications like video games, and, finally, explore the future possibilities in accelerating rendering.

## Background
Weekly progress can be found in the [Wiki](wiki).<br>
Rendered images can be found [here](Images).<br>


## Some Results

Diffuse scene with 500 samples per pixel.</br>
![Multisampling](Images/Test-Diffuse-500-Samples-per-Pixel.png?raw=true)</br></br>
Diffuse and metallic materials
![Diffuse and Metal](Images/Test-Specular-200-Samples-per-Pixel.png?raw=true)</br></br>
Perfect-mirror floor and a metallic Sphere
![Specular Reflection](Images/Specular-Reflection-Recursion-Depth-_-50.png)



