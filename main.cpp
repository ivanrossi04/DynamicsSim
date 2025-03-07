// !V1

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int axis;
static float axis_length = 200.0;
static float angle = 0.0;
static float zoom = 1.0;
static float cameraPos[3] = {0.0, 0.0, 0.0};

static float R = 5; // ball radius expressed in m
static const float mass = 0.01; // mass expressed in kg
static float currentPos[3] = {0.0, 0.0, 0.0}; // x_0 expressed in m
static float currentVel[3] = {0.0, 0.0, 0.0}; // v_0 expressed in m/s
static float currentTime = 0; // t_0 expressed in s

static float deltaTime = 0.001; // deltat expressed in s
static float deltaFrame = 0.02; // deltat expressed in s

std::vector<float*> trajectory;

// forces applied
static const float g = 9.806;
static const float k = 0.12;
float* f(float x[3], float v[3], float t) {
	return new float[3]{0.0, 0.0, 0.0};
}

// Routine to increase the rotation angle.
void increaseTime() {
	float* prevPos;
	currentTime += deltaTime;
	prevPos = currentPos;

	for (int i = 0; i < 3; i++) {
		float* force = f(prevPos, currentVel, currentTime);
		currentPos[i] = currentPos[i] + currentVel[i] * deltaTime;
		currentVel[i] = currentVel[i] + force[i] / mass * deltaTime;

		// std::cout << "t = " << currentTime << "\tx: " << currentPos[i] << ",\tv: " << currentVel[i] << "\n"; // debug statement
	}

	trajectory.push_back(new float[3] {currentPos[0], currentPos[1], currentPos[2]});
}

// Routine to draw a bitmap character string.
void writeBitmapString(void* font, char* string) {
	char* c;
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Routine to animate with a recursive call made after animationPeriod msecs.
int incrementCounter = 0;
void animate(int i) {
	increaseTime();
	incrementCounter++;

	if (incrementCounter >= deltaFrame / deltaTime) {
		glutPostRedisplay();
		incrementCounter = 0;
	}

	glutTimerFunc(deltaTime * 1000, animate, 1);
}

void drawScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glLoadIdentity();
	glTranslatef(cameraPos[0], cameraPos[1], cameraPos[2] -200.0);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glScalef(zoom, zoom, zoom);

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(2.0);

	// -- Draw Axis --
	glPushMatrix();
	glCallList(axis); // Execute display list: y axis
	glRasterPos3f(-10.0, axis_length - 10.0, - 10.0);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'y');

	glRotatef(-90, 0.0, 0.0, 1.0);
	glRasterPos3f(-10.0, axis_length - 10.0, -10.0);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'x');
	glCallList(axis); // Execute display list: x axis

	glRotatef(90, 1.0, 0.0, 0.0);
	glRasterPos3f(-10.0, axis_length - 10.0, -10.0);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'z');
	glCallList(axis); // Execute display list: z axis
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
	glutSolidSphere(5.0, 20, 20);
	

	glFlush();
}

// Initialization routine
void setup(void) {

	axis = glGenLists(1); // Return a list index.
	// Begin create a display list.
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
	// End create a display list.

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void resize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-75.0, 75.0, -75.0, 75.0, 100.0, 400.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
	case 'y':
		angle++;
		break;
	case 'Y':
		angle--;
		break;
	case '+':
		zoom*=1.1;
		break;
	case '-':
		zoom/=1.1;
		break;
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_DOWN:
		cameraPos[1]++;
		break;
	case GLUT_KEY_UP:
		cameraPos[1]--;
		break;
	case GLUT_KEY_LEFT:
		cameraPos[0]++;
		break;
	case GLUT_KEY_RIGHT:
		cameraPos[0]--;
		break;
	default:
		break;
	}
}

// Main routine.
int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Doing Physics!");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();
	animate(1);
	glutMainLoop();
}