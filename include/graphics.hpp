#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Graphics {

    extern unsigned int axis;

    void initGraphics(float axis_length = 200.0f);
    void deleteGraphics();
    void drawAxis();
    void drawSphere(float radius, glm::vec3 position, int slices = 20, int stacks = 20);
}

#endif