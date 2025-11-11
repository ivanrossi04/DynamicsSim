// !V1

#define _USE_MATH_DEFINES
#include <iostream>
#include <deque>

#include <glm/glm.hpp>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "camera.hpp"
#include "graphics.hpp"
#include "physics.hpp"

static CameraController camera;

const int MAX_POINTS = 10000;
std::deque<glm::vec3> trajectory;

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

    Graphics::drawSphere(5.0f, trajectory.back());

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

	static const float mass = 5.97219e8f; // mass of Earth in kg / 1e16
	static glm::vec3 currentPos(20.0f, 20.0f, 0.0f); // x_0 expressed in m
	static glm::vec3 currentVel(0.0f, -20.0f, 0.0f); // x_0 expressed in m/s

	Physics::Particle particle(mass, currentPos, currentVel);
	trajectory.push_back(currentPos);

	static float currentTime = 0; // t_0 expressed in s
	static float deltaTime = 0.001f; // deltat expressed in s
	static float deltaFrame = 0.02f; // deltat expressed in s

	Physics::Force* force = new Physics::GravitationalForce(1.98847e14f, mass); // mass of Sun in kg / 1e16
	particle.appliedForces.addForce(*force);

	// Enter the update cycle
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;

		lastTime = currentTime;
		accumulator += frameTime;

		while (accumulator >= deltaTime) {
			Propagation::rungeKutta4(particle, currentTime, deltaTime);

			if(trajectory.size() >= MAX_POINTS) trajectory.pop_front();
			trajectory.push_back(particle.getPosition());

			currentTime += deltaTime;
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
