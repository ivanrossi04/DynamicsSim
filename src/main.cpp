// !V1

#define _USE_MATH_DEFINES
#include <iostream>
#include <deque>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "graphics.hpp"

static CameraController camera;

static float R = 5; // ball radius expressed in m
static const float mass = 1; // mass expressed in kg
static glm::vec3 currentPos(55.0f, 0.0f, 0.0f); // x_0 expressed in m
static glm::vec3 currentVel(70.0f, 70.0f, 0.0f); // x_0 expressed in m/s
static float currentTime = 0; // t_0 expressed in s

static float deltaTime = 0.001f; // deltat expressed in s
static float deltaFrame = 0.02f; // deltat expressed in s

const int MAX_POINTS = 10000;
std::deque<glm::vec3> trajectory;

// forces applied
static const float g = 9.806f;
static const float k = 1.2f;
static const float G = 900000.0f;
glm::vec3 f(glm::vec3 x, glm::vec3 v, float t) {
    // return -k * v + glm::vec3(0.0f, -mass * g, 0.0f); // gravitational force with air resistance;
    // return -k * x; // hooke's law (spring constant is equal in every direction)
    return (-G / (float)pow(glm::length(x), 3)) * x; // kepler's problem
}

// Routine to increase the time of the simulation
void increaseTime() {
    // Runge - Kutta 4
	if(trajectory.size() >= MAX_POINTS) trajectory.pop_front();
    trajectory.push_back(currentPos);
    static glm::vec3 kx[4], kv[4];

    kx[0] = currentVel;
    static glm::vec3 force = f(currentPos, kx[0], currentTime);
    kv[0] = force * (1.0f / mass);

    kx[1] = currentVel + kv[0] * (deltaTime / 2.0f);
    force = f(currentPos + kx[0] * (deltaTime/2.0f), kx[1], currentTime + deltaTime / 2.0f);
    kv[1] = force * (1.0f / mass);

    kx[2] = currentVel + kv[1] * (deltaTime / 2.0f);
    force = f(currentPos + kx[1] * (deltaTime / 2.0f), kx[2], currentTime + deltaTime / 2.0f);
    kv[2] = force * (1.0f / mass);

    kx[3] = currentVel + kv[2] * deltaTime;
    force = f(currentPos + kx[2] * deltaTime, kx[3], currentTime + deltaTime);
    kv[3] = force * (1.0f / mass);

    currentTime += deltaTime;
    currentPos += deltaTime / 6.0f * (kx[0] + 2.0f * kx[1] + 2.0f * kx[2] + kx[3]);
    currentVel += deltaTime / 6.0f * (kv[0] + 2.0f * kv[1] + 2.0f * kv[2] + kv[3]);
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    camera.drawCamera();

    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2.0);

	Graphics::drawAxis();

    // -- Draw Trajectory --
    size_t size = trajectory.size();
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    for (size_t i = 0; i < size; i++) {
        glVertex3f(trajectory[i].x, trajectory[i].y, trajectory[i].z);
    }
    glEnd();

    Graphics::drawSphere(R, currentPos);

    glFlush();
}

// Initialization routine
void setup() {
	Graphics::initGraphics();
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void resize(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// set aspect ratio to avoid deformations
	float aspect = (float)w / (float)h;
	float viewSize = 75.0f;
	if (aspect >= 1.0f) glFrustum(-viewSize * aspect, viewSize * aspect, -viewSize, viewSize, 100.0, 400.0);
	else glFrustum(-viewSize, viewSize, -viewSize / aspect, viewSize / aspect, 100.0, 400.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main() {
	// Initialize the window
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Doing Physics!", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	setup();

	glfwSetFramebufferSizeCallback(window, resize);
	resize(window, 600, 600); // Set initial viewport size
	
	// Setup animation variables
	// Animation timing
	double lastTime = glfwGetTime();
	double accumulator = 0.0;

	camera = CameraController(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.025f, 0.05f, 0.001f);
	
	// Enter the update cycle
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;

		lastTime = currentTime;
		accumulator += frameTime;

		while (accumulator >= deltaTime) {
			increaseTime();
			accumulator -= deltaTime;
		}


		camera.moveCamera(window);
		drawScene();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Graphics::deleteGraphics();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
