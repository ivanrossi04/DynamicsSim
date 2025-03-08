
## Newtonian dynamics simulator
This is a personal project I'm making to learn computational physics. 
It is currently based on the simplest form of approximation in numerical integration: the Explicit Euclidean Algorithm.
It is a new project, so the code right now is a bit messy.

### Dependencies
For the computer graphics side, I'm using the OpenGL specification, alongside the [GLEW](https://github.com/nigels-com/glew) and [FreeGLUT](https://github.com/freeglut/freeglut) Libraries to render the objects on the screen.
Also using my linear algebra library, [Mathss-Library](https://github.com/ivanrossi04/Math-Library), to compute vector operations in a cleaner way, making the code more readable.

### TODO LIST:
- Find and implement an approximation algorithm that allows for conservation of energy
- Implement a system that allows the user to apply forces at runtime
- Deal with corner cases (what to do when the material point exits the virtual space, ...) 

Part 2: Deal with more particles :)