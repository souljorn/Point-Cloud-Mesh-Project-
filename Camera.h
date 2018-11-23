//-------------------------------------------------------------
// Camera.h
// extended from learnopengl.com 
// The camera is in a continuous orbit.
// Place the mouse over the window to draw the scene.
// Move the mouse over the window to rotate the object in the scene.
// The escape key exits
//-------------------------------------------------------------





#pragma once

#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>




enum MOVE_DIR {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:


                        Camera				();
                        Camera				(glm::vec3 posin, glm::vec3 wupin, GLfloat yawin, GLfloat pitchin);

                    
    glm::mat4			viewMat				(void);

    void				mouseIn				(GLfloat deltaX, GLfloat deltaY, GLboolean cPitch = true);
    void				mouseScroll			(GLfloat deltaY);

    void				keyIn				(MOVE_DIR direction, GLfloat deltaTime);


    GLfloat				delta;
    GLfloat				delta_mouse;
    GLfloat				zoomfactor;
    GLfloat				yaw;
    GLfloat				pitch;


    glm::vec3			pos;
    glm::vec3			up;
    glm::vec3			right;
    glm::vec3			dir;
    glm::vec3			wup;


private:

    void				update				(void);
};