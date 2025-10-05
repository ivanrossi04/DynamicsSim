
## Newtonian dynamics simulator
This is a personal project I'm making to learn computational physics.
The program is based on Runge-Kutta 4, a numerical integration algorithm which allows for fairly precise dynamics in the simple cases that this software wants to simulate.
It is a new project, so the code right now is a bit messy.

### Dependencies
For the computer graphics side, I'm using the OpenGL specification, alongside the [GLFW](https://github.com/glfw/glfw) and [GLAD](https://github.com/Dav1dde/glad) Libraries to render the objects on the screen.
Also using [GLM](https://github.com/g-truc/glm), to compute vector operations for graphics and physics computation.

### TODO LIST
==Fix display issues==
- [ ] Camera rotation: the camera should be able to move and rotate in all directions
- [x] Camera smoothness: right now it feels a bit clunky in it's movement, so I might implement some sort of smoothing in the movement speed
- [x] Window resizing: resizing the window streches out the displayed scene, which is not intended
- [ ] ...

==Implement a system that allows the user to apply forces at runtime (possibly using ImGui library to display interactable elements)==
- [ ] Make labels to display useful information about the dynamics (position, speed, acceleration)
- [ ] Make play / pause buttons
- [ ] Make an info banner to show the camera commands
- [ ] Implement another page banner where forces can be applied (ideally it should be a drag & drop feature, but more precise prerequisites or custom forces should be possible to do)

==Deal with corner cases (what to do when the material point exits the virtual space, deltaTime management) and debug if needed==

**Part 2:** Deal with more particles :)