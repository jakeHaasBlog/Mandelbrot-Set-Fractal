# Mandelbrot-Set-Fractal
The mandelbrot set rendered using mass parallelism in the graphics card. The code that you probably care about is in src/game/GameLogicInterface.cpp, there are seperate functions for generating the set via the CPU and GPU.  

Controls  
1 - render performed with GPU  
2 - render performed with CPU  

9 - decrease process itterations  
0 - increase process itterations  

W - move view up  
A - move view left  
S - move view down  
D - move view right  

Q - zoom out from mouse  
E - zoom in to mouse  

O - zoom out from middle screen  
P - zoom in to middle screen  

K - decrease color shift factor  
L - increase color shift factor  

Control + S - save a png image of the content pane of the window called "mandelbrot-image.png" in the root folder  


Libraries used
- GLFW - to create a window, OpenGLcontext, and to handke input
- GLEW - to wrangle OpenGL functions and extensions
- Stb_Image - to produce a png file from raw pixel data  


How to build  
The project is built with Microsoft's Visual Studio 2019. Simply clone the repo, open the Raycasting.sln file, then build the solution.


For more information about the Mandelbrot Set you can read about it here: https://en.wikipedia.org/wiki/Mandelbrot_set

