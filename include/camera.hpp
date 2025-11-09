// !V1
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @class CameraController
 * @brief Controls the camera in the 3D space
 * 
 * This class allows to control the camera position, angle and zoom in a 3D space.
 * It provides methods to get and set the camera parameters, as well as keyboard commands to move the camera.
 * 
 */
class CameraController {
	private:
		glm::vec3 cameraPos;
		glm::vec3 angle;
		float zoom;

		float cameraSpeed;
		float angularSpeed;
		float zoomSpeed;

	public:

		/**
		 * @brief Constructor for CameraController
		 * 
		 * @param cameraPos Initial position of the camera
		 * @param angle Initial angle of the camera
		 * @param zoom Initial zoom level of the camera
		 * @param cameraSpeed Speed of camera movement
		 * @param angularSpeed Speed of camera rotation
		 * @param zoomSpeed Speed of camera zoom
		 * 
		 * @note Default values are provided for all parameters
		 */
		CameraController(glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 angle = glm::vec3(0.0f, 0.0f, 0.0f), float zoom = 1.0f, float cameraSpeed = 0.0f, float angularSpeed = 0.0f, float zoomSpeed = 0.0f);
		
		glm::vec3 getPosition();
		glm::vec3 getAngle();
		float getZoom();

		void setPosition(glm::vec3 pos);
		void setAngle(float angle);
		void setZoom(float zoom);

		void setCameraSpeed(float speed);
		void setAngularSpeed(float speed);
		void setZoomSpeed(float speed);

		/**
		* @brief Update the camera movement based on user input
		* 
		* This method checks for keyboard inputs and updates the camera position, angle, and zoom accordingly.
		*
		* The camera moves forward, backward, left, and right using the W, S, A, and D keys respectively.
		* The camera rotates around the X, Y, and Z axes using the X, Y, and Z keys respectively, with the Shift key modifying the direction of rotation.
		* The zoom level can be adjusted using the Up and Down arrow keys.
		*
		* @param window Pointer to the GLFW window
		*/
		void moveCamera(GLFWwindow* window);

		/**
		 * @brief Draw the camera view
		 * 
		 * This method renders the camera view in the 3D space, applying the current position, angle, and zoom as transformations.
		 * Call this method before rendering the scene to ensure the camera view is correctly set up.
		 */
		void drawCamera();
};

#endif
