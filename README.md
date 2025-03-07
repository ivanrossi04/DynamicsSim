
## Newtonian dynamics simulator
This is a personal project I'm making to learn computational physics. 
It is currently based on the simplest form of approximation in numerical integration: the Explicit Euclidean Algorithm.
It is a new project, so the code right now is a bit messy.

### Dependencies
I'm using the OpenGL specification, alongside the [GLEW](https://github.com/nigels-com/glew) and [FreeGLUT](https://github.com/freeglut/freeglut) Libraries to render the objects on the screen.

### TODO LIST:
- Refactor code using Mathss Library (Focus on scalability)
- Find and implement an approximation algorithm that allows for conservation of energy
- Implement a system that allows the user to apply forces at runtime
- Deal with corner cases (what to do when the material point exits the virtual space, ...) 

Part 2: Deal with more particles :)