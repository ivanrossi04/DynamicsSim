// !V1
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraController {
	private:
		glm::vec3 cameraPos;
		glm::vec3 angle;
		float zoom;

		float cameraSpeed;
		float angularSpeed;
		float zoomSpeed;

	public:
		CameraController(glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 angle = glm::vec3(0.0f, 0.0f, 0.0f), float zoom = 1.0f, float cameraSpeed = 0.0f, float angularSpeed = 0.0f, float zoomSpeed = 0.0f) {
			this->cameraPos = cameraPos;
			this->angle = angle;
			this->zoom = zoom;
			this->cameraSpeed = cameraSpeed;
			this->angularSpeed = angularSpeed;
			this->zoomSpeed = zoomSpeed;
		}

		glm::vec3 getPosition() { return cameraPos; }
		glm::vec3 getAngle() { return angle; }
		float getZoom() { return zoom; }

		void setPosition(glm::vec3 pos) { cameraPos = pos; }
		void setAngle(float angle) { angle = angle; }
		void setZoom(float zoom) { this->zoom = zoom; }

		void setCameraSpeed(float speed) { cameraSpeed = speed; }
		void setAngularSpeed(float speed) { angularSpeed = speed; }
		void setZoomSpeed(float speed) { zoomSpeed = speed; }

		void moveCamera(GLFWwindow* window) {
			// movement along x and y axis
			bool horizontal = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ^ (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
			bool vertical = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ^ (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);

			float cameraSpeed = this->cameraSpeed;
			if (horizontal && vertical) cameraSpeed *= 0.7071f; // sqrt(2) / 2

			if (horizontal) cameraPos.x += cameraSpeed * (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ? 1.0f : -1.0f);
			if (vertical) cameraPos.y += cameraSpeed * (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? 1.0f : -1.0f);

			// rotation around y axis
			bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
			if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) angle.x += angularSpeed * (shiftPressed ? 1.0f : -1.0f);
			if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) angle.y += angularSpeed * (shiftPressed ? 1.0f : -1.0f);
			if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) angle.z += angularSpeed * (shiftPressed ? 1.0f : -1.0f);

			// zoom
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) zoom *= 1 + zoomSpeed;
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) zoom *= 1 - zoomSpeed;
		}

		void drawCamera() {
			glLoadIdentity();
			glTranslatef(cameraPos.x, cameraPos.y, -200.0);
			glRotatef(angle.x, 1.0f, 0.0f, 0.0f); // Rotate around X axis
			glRotatef(angle.y, 0.0f, 1.0f, 0.0f); // Rotate around Y axis
			glRotatef(angle.z, 0.0f, 0.0f, 1.0f); // Rotate around Z axis

			glScalef(zoom, zoom, zoom);
		}
};

#endif