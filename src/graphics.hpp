#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Graphics {

    unsigned int axis = 0;

    void initGraphics(float axis_length = 200.0f) {
        axis = glGenLists(1);
        glNewList(axis, GL_COMPILE);
        glBegin(GL_LINES);
        
        // set color
        glColor3f(0.0, 0.0, 0.0);
        
        // Draw the axis Line
        glVertex3f(0.0, -axis_length, 0.0);
        glVertex3f(0.0, axis_length, 0.0);

        // Draw step lines at regular intervals (every 10m)
        float width;
        for (size_t i = 0; i <= axis_length / 5; i++) {
            width = i % 5 ? 2.0f : 4.0f;
            glVertex3f(-width, -axis_length + i * 10.0f, 0.0f);
            glVertex3f(width, -axis_length + i * 10.0f, 0.0f);

            glVertex3f(0.0f, -axis_length + i * 10.0f, -width);
            glVertex3f(0.0f, -axis_length + i * 10.0f, width);
        }

        glEnd();
        glEndList();
    }

    void deleteGraphics() {
        glDeleteLists(axis, 1);
    }

    void drawAxis() {
        glPushMatrix();
        glCallList(axis); // x axis
        glRotatef(-90, 0.0, 0.0, 1.0);
        glCallList(axis); // y axis
        glRotatef(90, 1.0, 0.0, 0.0);
        glCallList(axis); // z axis
        glPopMatrix();
    }

    void drawSphere(float radius, glm::vec3 position, int slices = 20, int stacks = 20) {
        glTranslatef(position.x, position.y, position.z);

        for (int i = 0; i < stacks; ++i) {
            float lat0 = M_PI * (-0.5 + (float)i / stacks);
            float z0 = sinf(lat0), zr0 = cosf(lat0);

            float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);
            float z1 = sinf(lat1), zr1 = cosf(lat1);

            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= slices; ++j) {
                float lng = 2 * M_PI * (float)(j - 1) / slices;
                float x = cosf(lng), y = sinf(lng);

                glNormal3f(x * zr0, y * zr0, z0);
                glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
                glNormal3f(x * zr1, y * zr1, z1);
                glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
            }
            glEnd();
        }
    }
}

#endif