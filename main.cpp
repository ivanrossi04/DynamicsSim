// !V1

#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include "mathss.hpp"
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int axis;
static float axis_length = 200.0;

static unsigned int square;
static float border_width = 0.1;

void buildCustomShapes() {

	axis = glGenLists(1);
	glNewList(axis, GL_COMPILE);
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, -axis_length, 0.0);
	glVertex3f(0.0, axis_length, 0.0);

	float width;
	for (size_t i = 0; i <= axis_length / 5; i++) {
		width = i % 5 ? 2.0 : 4.0;
		glVertex3f(-width, -axis_length + i * 10.0, 0.0);
		glVertex3f(width, -axis_length + i * 10.0, 0.0);

		glVertex3f(0.0, -axis_length + i * 10.0, -width);
		glVertex3f(0.0, -axis_length + i * 10.0, width);
	}
	glEnd();
	glEndList();

	square = glGenLists(1);
	glNewList(square, GL_COMPILE);
	glBegin(GL_POLYGON);
	glVertex3f(-1.0, -1.0, 0.0);
	glVertex3f(1.0, -1.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glVertex3f(-1.0, 1.0, 0.0);
	glEnd();
	glEndList();
}


static float angle = 0.0;
static float zoom = 1.0;
static Vec<float> cameraPos(3, 0.0);

static float R = 5; // ball radius expressed in m
static const float mass = 1; // mass expressed in kg
static Vec<float> currentPos({ 50.0, 50.0, 0.0 }); // x_0 expressed in m
static Vec<float> currentVel({50.0, -50.0, 0.0}); // x_0 expressed in m/s
static float currentTime = 0; // t_0 expressed in s

static float deltaTime = 0.01; // deltat expressed in s
static float deltaFrame = 0.02; // deltat expressed in s

std::vector<Vec<float>> trajectory;

// forces applied
static const float g = 9.806;
static const float k = 1.2;
static const float G = 900000;
Vec<float> f(Vec<float> x, Vec<float> v, float t) {
	// return -k * v + Vec<float>({0.0, -mass * g, 0.0}); // gravitational force with air resistance;
	// return - k * x; // hooke's law (spring costant is equal in every direction)
	return (- G / (float)pow(sqrt(x.getNorm()), 3)) * x; // kepler's problem
}

// Routine to increase the rotation angle.
void increaseTime() {
	/* explicit eluer method
	Vec<float> prevPos = currentPos;
	trajectory.push_back(prevPos);

	currentTime += deltaTime;
	Vec<float> force = f(prevPos, currentVel, currentTime);
	currentPos += currentVel * deltaTime;
	currentVel += force * (1 / mass) * deltaTime;
	*/

	/* midpoint rule(RK2)
	trajectory.push_back(Vec<float>(currentPos));
	Vec<float> k1x, k1v, k2x, k2v;

	Vec<float> force = f(currentPos, currentVel, currentTime);
	k1x = currentVel;
	k1v = force * (1 / mass);

	Vec<float> nextPos = currentPos + deltaTime * k1x;
	Vec<float> nextVel = currentVel + deltaTime * k1v;

	force = f(nextPos, nextVel, currentTime + deltaTime);
	k2x = nextVel;
	k2v = force * (1 / mass);


	currentPos += (k1x + k2x) * (deltaTime / 2);
	currentVel += (k1v + k2v) * (deltaTime / 2);
	*/

	// Runge-Kutta 4
	trajectory.push_back(Vec<float>(currentPos));
	static Vec<float> kx[4], kv[4];

	kx[0] = currentVel;
	static Vec<float> force = f(currentPos, kx[0], currentTime);
	kv[0] = force * (1 / mass);

	kx[1] = currentVel + kv[0] * (deltaTime / 2);
	force = f(currentPos + kx[0] * (deltaTime/2), kx[1], currentTime + deltaTime / 2);
	kv[1] = force * (1 / mass);

	kx[2] = currentVel + kv[1] * (deltaTime / 2);
	force = f(currentPos + kx[1] * (deltaTime / 2), kx[2], currentTime + deltaTime / 2);
	kv[2] = force * (1 / mass);

	kx[3] = currentVel + kv[2] * deltaTime;
	force = f(currentPos + kx[2] * deltaTime, kx[3], currentTime + deltaTime);
	kv[3] = force * (1 / mass);

	currentTime += deltaTime;
	currentPos += deltaTime / 6 * (kx[0] + 2.0f * kx[1] + 2.0f * kx[2] + kx[3]);
	currentVel += deltaTime / 6 * (kv[0] + 2.0f * kv[1] + 2.0f * kv[2] + kv[3]);
}

void drawSolidSphere(float radius, int slices, int stacks) {
	for (int i = 0; i < stacks; ++i) {
		float lat0 = M_PI * (-0.5 + (float)i / stacks);
		float z0 = sin(lat0), zr0 = cos(lat0);

		float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);
		float z1 = sin(lat1), zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= slices; ++j) {
			float lng = 2 * M_PI * (float)(j - 1) / slices;
			float x = cos(lng), y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
		}
		glEnd();
	}
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glLoadIdentity();
	glTranslatef(cameraPos[0], cameraPos[1], cameraPos[2] - 200.0);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glScalef(zoom, zoom, zoom);

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);

	// -- Draw Axis --
	glPushMatrix();
	glCallList(axis); // Execute display list: y axis

	glRotatef(-90, 0.0, 0.0, 1.0);
	glCallList(axis); // x axis

	glRotatef(90, 1.0, 0.0, 0.0);
	glCallList(axis); // z axis
	glPopMatrix();

	// -- Draw Trajectory --
	size_t size = trajectory.size();
	glBegin(GL_LINE_STRIP);
	glColor3f(1.0, 0.0, 0.0);
	for (size_t i = 0; i < size; i++) {
		glVertex3f(trajectory[i][0], trajectory[i][1], trajectory[i][2]);
	}
	glEnd();

	// -- Draw Ball --
	glTranslatef(currentPos[0], currentPos[1], currentPos[2]);
	drawSolidSphere(5.0, 20, 20);

	glFlush();
}

// Initialization routine
void setup() {
	buildCustomShapes();
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void resize(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-75.0, 75.0, -75.0, 75.0, 100.0, 400.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_Y:
			if (mods & GLFW_MOD_SHIFT)
				angle--;
			else
				angle++;
			break;
		case GLFW_KEY_KP_ADD:
		case GLFW_KEY_EQUAL:
			zoom *= 1.1;
			break;
		case GLFW_KEY_KP_SUBTRACT:
		case GLFW_KEY_MINUS:
			zoom /= 1.1;
			break;
		case GLFW_KEY_DOWN:
			cameraPos[1]++;
			break;
		case GLFW_KEY_UP:
			cameraPos[1]--;
			break;
		case GLFW_KEY_LEFT:
			cameraPos[0]++;
			break;
		case GLFW_KEY_RIGHT:
			cameraPos[0]--;
			break;
		default:
			break;
		}
	}
}

int main() {
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	setup();

	glfwSetFramebufferSizeCallback(window, resize);
	resize(window, 600, 600); // Set initial viewport size

	glfwSetKeyCallback(window, keyInput);

	// Animation timing
	double lastTime = glfwGetTime();
	double accumulator = 0.0;

	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		double frameTime = currentTime - lastTime;

		lastTime = currentTime;
		accumulator += frameTime;

		while (accumulator >= deltaTime) {
			increaseTime();
			accumulator -= deltaTime;
		}

		drawScene();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}