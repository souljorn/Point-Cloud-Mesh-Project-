//-------------------------------------------------------------n 
// Camera.cpp
// extended from learnopengl.com 
// The camera is in a continuous orbit.
// Place the mouse over the window to draw the scene.
// Move the mouse over the window to rotate the object in the scene.
// The escape key exits
//-------------------------------------------------------------



#include "Camera.h"



//------------------------------------------
// Initialize Camera Parameters
//------------------------------------------
Camera::Camera()
	:	delta			(3.0f), 
		delta_mouse		(0.25f),
		zoomfactor		(45.0f),
		yaw				(-90.0f),
		pitch			(0.0f),
		pos				(glm::vec3(0.0f, 0.0f, 0.0f)),
		wup				(glm::vec3(0.0f, 1.0f, 0.0f)),
		dir				(glm::vec3(0.0f, 0.0f, -1.0f))
{
}


//------------------------------------------
// Initialize Camera Parameters
//------------------------------------------
Camera::Camera(glm::vec3 posin, glm::vec3 wupin, GLfloat yawin, GLfloat pitchin)
	:	delta			(3.0f),
		delta_mouse		(0.25f),
		zoomfactor		(45.0f),
		yaw				(yawin),
		pitch			(pitchin),
		pos				(posin),
		wup				(wupin),
		dir				(glm::vec3(0.0f, 0.0f, -1.0f))
{
}



//------------------------------------------
// Get view matrix
//------------------------------------------
glm::mat4 Camera::viewMat()
{
	return glm::lookAt(pos, pos + dir, up);
}




//------------------------------------------
// Process Mouse Input
//------------------------------------------
void Camera::mouseIn(GLfloat deltaX, GLfloat deltaY, GLboolean cPitch)
{
	deltaX *= delta_mouse;
	deltaY *= delta_mouse;
	yaw += deltaX;
	pitch += deltaY;


	if (cPitch)
	{
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
			
	}

	update();
}



//------------------------------------------
// Mouse Scroll
//------------------------------------------
void Camera::mouseScroll(GLfloat deltaY)
{
	if (zoomfactor >= 1.0f && zoomfactor <= 45.0f)
	{
		zoomfactor -= deltaY/10;
	}
	if (zoomfactor <= 1.0f)
	{
		zoomfactor = 1.0f;
	}
	if (zoomfactor >= 45.0f)
	{
		zoomfactor = 45.0f;
	}
}

//------------------------------------------
// Key press events
//------------------------------------------
void Camera::keyIn(MOVE_DIR direction, GLfloat deltaTime)
{
	GLfloat velocity = delta * deltaTime;
	if (direction == FORWARD)
	{
		pos += dir * velocity;
	}
	if (direction == BACKWARD)
	{
		pos -= dir * velocity;
	}
	if (direction == LEFT)
	{
		pos -= right * velocity;
	}
	if (direction == RIGHT)
	{
		pos += right * velocity;
	}
	if (direction == UP)
	{
		pos += up * velocity;
	}
	if (direction == DOWN)
	{
		pos -= up * velocity;
	}
}


//------------------------------------------
// update camera parameters
//------------------------------------------
void Camera::update(void)
{
	glm::vec3 lookVec;
	lookVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	lookVec.y = sin(glm::radians(pitch));
	lookVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	

	dir = glm::normalize(lookVec);
	right = glm::normalize(glm::cross(dir, wup));
	up = glm::normalize(glm::cross(right, dir));
}