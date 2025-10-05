// !V1
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraController {
	private:
		glm::vec3 cameraPos;
		float angleY;
		float zoom;

		float cameraSpeed;
		float angularSpeed;
		float zoomSpeed;

	public:
		CameraController(glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f), float angleY = 0.0f, float zoom = 1.0f, float cameraSpeed = 0.0f, float angularSpeed = 0.0f, float zoomSpeed = 0.0f) {
			this->cameraPos = cameraPos;
			this->angleY = angleY;
			this->zoom = zoom;
			this->cameraSpeed = cameraSpeed;
			this->angularSpeed = angularSpeed;
			this->zoomSpeed = zoomSpeed;
		}

		glm::vec3 getPosition() { return cameraPos; }
		float getAngle() { return angleY; }
		float getZoom() { return zoom; }

		void setPosition(glm::vec3 pos) { cameraPos = pos; }
		void setAngle(float angle) { angleY = angle; }
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
			if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) angleY += angularSpeed * (shiftPressed ? 1.0f : -1.0f);

			// zoom
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) zoom *= 1 + zoomSpeed;
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) zoom *= 1 - zoomSpeed;
		}

		void drawCamera() {
			glLoadIdentity();
			glTranslatef(cameraPos.x, cameraPos.y, -200.0);
			glRotatef(angleY, 0.0, 1.0, 0.0);
			glScalef(zoom, zoom, zoom);
		}
};

#endif