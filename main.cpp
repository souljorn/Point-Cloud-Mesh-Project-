//-------------------------------------------------------------
// University of Florida
// copyright 2017 Corey Toler-Franklin Tim Botelho David Cabrera Andrew 
// main.cpp for Mesh Creation 
//-------------------------------------------------------------

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <math.h>
#include "Shader.h"
#include "Camera.h"
#include "colors.h"
#include "Mesh.h"
#include "./Libraries/imgui/imgui.h"
#include "./Libraries/imgui/examples/imgui_impl_glfw.h"
#include "./Libraries/imgui/examples/imgui_impl_opengl3.h"


// david's magic header
#include "davidmagic.h"



class Mesh;
double find_Mod(double a, double b);
//----------------------------------------------
//		VAO/VBO/EBO 
//----------------------------------------------

enum VAO_IDs { PointCloud, Lines, Grid, GridLines,
	XYPlane, NormalsUO, QueryVao, NearestNeighborVAO, NumVAOs };
GLuint  VAOs[NumVAOs];
Shader * basicShaderProgram = new Shader();
Shader * pointShaderProgram = new Shader();

//----------------------------------------------
// Gloabal Meshes
//---------------------------------------------

Mesh * square;
Mesh * grid;
Mesh * pointCloud;
Mesh * gridLines;
Mesh * xzPlane;
Mesh * normalsUO;
Mesh * queryPointMesh;
Mesh * nearestNeighborMesh;
Mesh * nearestNeighborMesh1;


//----------------------------------------------
// Data Globals
//----------------------------------------------
bool showGui = false;
float PointSize = 1.0;
int numLines;
OutData data;
std::vector<Vertex> normalsUnordered;
std::vector<Vertex> queryPoints; 
std::vector<std::vector<Vertex>> nearestNeighbor;
bool norm = false;

//----------------------------------------------
// Shader ID's
//----------------------------------------------
GLuint modelIDBasic, viewIDBasic, projectionIDBasic, cameraIDBasic;
GLuint modelIDPoint, viewIDPoint, projectionIDPoint, cameraIDPoint;
GLuint alphaBasic;

//----------------------------------------------
// windows
//----------------------------------------------
int windowHeight = 2000;
int windowLength = 1400;

//----------------------------------------------
// Perspective settings
//----------------------------------------------
float fov = 50.0f;
float aspect = (float)windowLength / (float)windowHeight;
unsigned int index = 0;

//----------------------------------------------
// camera 
//----------------------------------------------
// initializes the camera. The teapot is a Geometry object.
Camera camera(glm::vec3(.50f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
float camX = -0.8f;
float camY = -2.8f;
float camZ = 30.0f;

float lookX = 0.024f;
float lookY = 0.049f;
float lookZ = -0.065f;

//----------------------------------------------
// time variables
//----------------------------------------------
// tracks the change in time for camera and model
// manipulation in the scene
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//----------------------------------------------
// Callback functions
//----------------------------------------------
// manages the mouse position which controls the camera
// and rotates the teapot model
void KeyCallback(GLFWwindow *window, int key, int scan, int act, int mode);
void MouseCallback(GLFWwindow *window, double xPosition, double yPosition);
void ScrollCallback(GLFWwindow *window, double deltaX, double deltaY);
void KeyMovement(void);
bool keys[1024];
GLfloat lastX = windowLength / 2, lastY = windowHeight / 2;
bool firstMouse = true;

//----------------------------------------------
// Display
//----------------------------------------------
// continually draws the scene
void display(int windowWidth, int windowHeight);

//----------------------------------------------
// Clean UP Resources
//----------------------------------------------
void cleanUp()
{
	delete grid;
	delete pointCloud;
	delete square;
	delete gridLines;
	delete xzPlane;
}

void parseData()
{
	alglib::ae_int_t * tag ;
	int temp;
	int cols;
	int rows;
	//for (auto d : data.kdTreeData) {
	//	int i = std::addressof(d) - std::addressof(data.kdTreeData[0]);
	//	std::cout << "For query point " << d.queryPoint.tostring(constants::psd) << " with kRadius " << data.kRadius << std::endl;
	//	std::cout << "The neighborhood is the set " << d.neighbors.tostring(constants::psd) << std::endl;
	//	std::cout << "The centroid is " << d.centroid.tostring(constants::psd) << std::endl;
	//	std::cout << "And the normal is " << d.normal.tostring(constants::psd) << std::endl;
	//	std::cout << "With tag index " << d.tagsCentroids[i] << std::endl << std::endl;

	//	tag = d.tagsCentroids.getcontent();
	//	 temp = *tag;
	//}

	double *norm;
	double * qp;
	for (int i = 0; i < data.nPoints ; i++) {
		//int i = std::addressof(d) - std::addressof(data.kdTreeData[0]);
		//std::cout << "For query point " << d.queryPoint.tostring(constants::psd) << " with kRadius " << data.kRadius << std::endl;
		//std::cout << "The neighborhood is the set " << data.kdTreeData.at(i).neighbors.tostring(constants::psd) << std::endl;
		//std::cout << "The centroid is " << d.centroid.tostring(constants::psd) << std::endl;
		//std::cout << "And the normal is " << d.normal.tostring(constants::psd) << std::endl;
		//std::cout << "With tag index " << d.tagsCentroids[i] << std::endl << std::endl;
		
		
		//std::cout << "With tag index " << data.kdTreeData.at(i).tagsCentroids[i] << std::endl << std::endl;
		//tag = data.kdTreeData.at(i).tagsCentroids.getcontent();
		//temp = *tag;
		//std::cout << "Centroid Tag:" << temp << std::endl;

		//std::cout << "And the normal is " << data.kdTreeData.at(i).normal.tostring(constants::psd) << std::endl;
		//long len = (long)data.kdTreeData.at(i).normal.length();
		cols = data.kdTreeData.at(i).neighbors.cols();
		rows = data.kdTreeData.at(i).neighbors.rows();
		//std::cout << "Colums:"<<cols << "Rows:"<<rows << std::endl;

		std::vector<Vertex> temp;
		//std::cout << "nearest neighbor:" << std::endl;
		for(int j = 0; j < rows; j++)
		{
			temp.push_back(Vertex(data.kdTreeData.at(i).neighbors[j][0], data.kdTreeData.at(i).neighbors[j][1], data.kdTreeData.at(i).neighbors[j][2], Red));
			
			//std::cout << "(" << data.kdTreeData.at(i).neighbors[j][0] << "," << data.kdTreeData.at(i).neighbors[j][1] << "," << data.kdTreeData.at(i).neighbors[j][2] << ") ,";
		}
		nearestNeighbor.push_back(temp);
		//next = next + data.kdTreeData.at(i).neighbors.getstride();
		//std::cout << std::endl;
		norm = data.kdTreeData.at(i).normal.getcontent();
		normalsUnordered.push_back(Vertex((float)norm[0], (float)norm[1], (float)norm[2], Violet));
		
		qp = data.kdTreeData.at(i).queryPoint.getcontent();
		queryPoints.push_back(Vertex((float)qp[0], (float)qp[1], (float)qp[2], Violet));
	}

	

	// all three pointers point to the same location
	//double *b_row0 = &b[0][0];
	//double *b_row0_2 = &b(0, 0);
	//double *b_row0_3 = b[0];

	// advancing to the next row of 2-dimensional array
	//double *b_row1 = b_row0 + b.getstride();
}

//----------------------------------------------
// init
//----------------------------------------------
void init()
{
	parseData();
	//-----Open GL Options---------
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_POLYGON_SMOOTH);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);
	glPointSize(PointSize);

	//--------Load Shader-----------
	static Shader basicShader("./Shaders/basic.vert", "./Shaders/basic.frag");
	basicShaderProgram = &basicShader;
	static Shader pointCloudShader("./Shaders/point.vert", "./Shaders/point.frag");
	pointShaderProgram = &pointCloudShader;

	//Setup link between Shaders and Uniforms
	GLCall(modelIDBasic = glGetUniformLocation(basicShaderProgram->Program, "model"));
	GLCall(viewIDBasic = glGetUniformLocation(basicShaderProgram->Program, "view"));
	GLCall(projectionIDBasic = glGetUniformLocation(basicShaderProgram->Program, "projection"));
	GLCall(cameraIDBasic = glGetUniformLocation(basicShaderProgram->Program, "camera"));
	GLCall(alphaBasic = glGetUniformLocation(basicShaderProgram->Program, "alpha"));

	//Setup link between Shaders and Uniforms
	GLCall(modelIDPoint = glGetUniformLocation(pointShaderProgram->Program, "model"));
	GLCall(viewIDPoint = glGetUniformLocation(pointShaderProgram->Program, "view"));
	GLCall(projectionIDPoint= glGetUniformLocation(pointShaderProgram->Program, "projection"));
	GLCall(cameraIDPoint = glGetUniformLocation(pointShaderProgram->Program, "camera"));
	GLCall(alphaBasic = glGetUniformLocation(pointShaderProgram->Program, "alpha"));



	// Setup of buffers for the mesh objects
	// ------------------------------------------------------------------

	pointCloud = new Mesh();
	pointCloud->createPointCloud("./PointClouds/face.obj");
	pointCloud->createBufferPoints(VAOs[PointCloud]);

	square = new Mesh();
	square->createSquare();
	square->createBuffers(VAOs[Lines]);

	grid = new Mesh();
	grid->createGrid(4, .0f, 0.5f, 0.0f, 0.5f, Zaxis, Orange);
	grid->createBufferPoints(VAOs[Grid]);

	gridLines = new Mesh();
	gridLines->createGrid(4, .0f, 0.5f, 0.0f, 0.5f, Zaxis, Blue);
	gridLines->createBuffers(VAOs[GridLines]);

	//x
	xzPlane = new Mesh();
	xzPlane->createGrid(16, -2.0f, 2.0f, -2.0f, 2.0f, Yaxis, DarkSlateGray);
	xzPlane->createBuffers(VAOs[XYPlane]);

	normalsUO = new Mesh();
	normalsUO->createLines(queryPoints,normalsUnordered);
	normalsUO->createBuffers(VAOs[NormalsUO]);

	queryPointMesh = new Mesh();
	queryPointMesh->createPoints(queryPoints, Azure);
	queryPointMesh->createBufferPoints(VAOs[QueryVao]);

	nearestNeighborMesh = new Mesh();
	nearestNeighborMesh->createPoints(nearestNeighbor.at(0), Red);
	nearestNeighborMesh->createBufferPoints(VAOs[NearestNeighborVAO]);

	

}

//----------------------------------------------
// display function
//-----------------------------------------------
void display(int windowWidth, int windowHeight)
{	
	//// sets up the camera motion

    float ratio = (float)windowHeight/windowWidth;
	float degrees = (int)(80 * glfwGetTime()) % 360;

	//Time for dynamic rendering/animation
	static float time;
	time = glfwGetTime();
	 
	//Clear the buffer
	GLCall(glClearColor(0.8f, 0.8f, 0.8f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	
	//-------------------------
	//     Matrix Update
	//------------------------
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowLength) / static_cast<float>(windowHeight), .10f, 50.0f);

	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 view = glm::lookAt(
		glm::vec3(camX,camY,camZ), // Camera is at (4,3,3), in World Space
		glm::vec3(lookX, lookY, lookZ), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 modelGrid = glm::mat4(1.0f);
	modelGrid = glm::scale(modelGrid, glm::vec3(.05, .05, .05));
	modelGrid = glm::translate(modelGrid, glm::vec3(0, 0, 0));
	modelGrid = glm::rotate(modelGrid, 20.0f , glm::vec3(1, .2f, 0));

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 modelPoint = glm::mat4(1.0f);
	modelPoint = glm::scale(modelPoint, glm::vec3(.05, .05, .05));
	modelPoint = glm::translate(modelPoint, glm::vec3(0, 0, 0));
	modelPoint = glm::rotate(modelPoint, 20.0f, glm::vec3(1, .2f, 0));

	//-------------------------
	//     Activate Shader
	//------------------------
	
	basicShaderProgram->Use();
	

	//---------------Link Matrices to Basic Shader--------------------------------
	GLCall(glUniformMatrix4fv(modelIDBasic, 1, GL_FALSE, glm::value_ptr(modelGrid)));
	GLCall(glUniformMatrix4fv(viewIDBasic, 1, GL_FALSE, glm::value_ptr(view)));
	GLCall(glUniformMatrix4fv(projectionIDBasic, 1, GL_FALSE, glm::value_ptr(projection)));
	GLCall(glUniform1f(alphaBasic, sin(time * 1)));
	//gridLines->drawLinesSequence(time, gridLines->getNumIndices() + 1);
	//grid->drawPoints();
	xzPlane->drawLines(0, 0, 0);
	if (norm) {
		normalsUO->drawLines(0, 0, 0);
	}
	//queryPointMesh->drawPoints();
	nearestNeighborMesh->drawPoints();
	//nearestNeighborMesh1->drawPoints();



	//----------Mesh Draw Calls------------------------------------
	//square->drawLines(0, 0, 0);

	//---------------Link Matrices to Basic Shader--------------------------------
	pointShaderProgram->Use();

	GLCall(glUniformMatrix4fv(modelIDPoint, 1, GL_FALSE, glm::value_ptr(modelPoint)));
	GLCall(glUniformMatrix4fv(viewIDPoint, 1, GL_FALSE, glm::value_ptr(view)));
	GLCall(glUniformMatrix4fv(projectionIDPoint, 1, GL_FALSE, glm::value_ptr(projection)));
	
	//pointCloud->drawPoints();
	
	//gridLines->drawLines(0, 0, 0);
	
	GLCall(glDepthFunc(GL_LESS));
}


//=====================================================================
// Cameras and callbacks
//=====================================================================
// The camera is in a continuous orbit.
// Place the mouse over the window to draw the scene.
// Move the mouse over the window to rotate the object in the scene.
// The escape key exits
//-------------------------------------------------------------

void KeyCallback(GLFWwindow *window, int key, int scan, int act, int mode)
{
    //escape with escape key
	if (key == GLFW_KEY_ESCAPE && act == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);


    if (key == GLFW_KEY_UP && act == GLFW_PRESS)
    {
		camX +=0.1;
		std::cout << "camX:" << camX << std::endl;
    }

    if (key == GLFW_KEY_DOWN && act == GLFW_PRESS)
    {
		camX -= 0.1;
		std::cout << "camX:" << camX << std::endl;
    }

	if (key == GLFW_KEY_LEFT && act == GLFW_PRESS)
	{
		camY += .1;
		std::cout << "camY:" << camY << std::endl;
	}
    if (key == GLFW_KEY_RIGHT && act == GLFW_PRESS)
    {
		camY -= .1;
		std::cout << "camY:" << camY << std::endl;
    }

	if (key == GLFW_KEY_1 && act == GLFW_PRESS)
	{
		camZ += .1;
		std::cout << "camZ:" << camZ << std::endl;
	}

	if (key == GLFW_KEY_2 && act == GLFW_PRESS)
	{
		camZ -= .1;
		std::cout << "camZ:" << camZ << std::endl;
	}
	if (key == GLFW_KEY_3 && act == GLFW_PRESS)
	{
		lookX += .003;
		std::cout << "lookX:" << lookX << std::endl;
	}

	if (key == GLFW_KEY_4 && act == GLFW_PRESS)
	{
		lookX -= .003;
		std::cout << "lookX:" << lookX << std::endl;
	}

	if (key == GLFW_KEY_5 && act == GLFW_PRESS)
	{
		lookY += .003;
		std::cout << "lookY:" << lookY << std::endl;
	}

	if (key == GLFW_KEY_6 && act == GLFW_PRESS)
	{
		lookY -= .003;
		std::cout << "lookY:" << lookY << std::endl;
	}
	if (key == GLFW_KEY_7 && act == GLFW_PRESS)
	{
		lookZ += .003;
		std::cout << "lookZ:" << lookZ << std::endl;
	}

	if (key == GLFW_KEY_8 && act == GLFW_PRESS)
	{
		lookZ -= .003;
		std::cout << "lookZ:" << lookZ << std::endl;
	}
	if (key == GLFW_KEY_N && act == GLFW_PRESS)
	{
		norm = !norm;
		
	}
	

    //updating keys table 
    if (act == GLFW_PRESS)
        keys[key] = true;
    else if (act == GLFW_RELEASE)
        keys[key] = false;

}

// controlling camera ad object orientation
void MouseCallback(GLFWwindow *window, double xPosition, double yPosition)
{
    //first time being encountered
    if (firstMouse)
    {
        lastX = xPosition;
        lastY = yPosition;
        firstMouse = false;
    }
    
    //finding change
    GLfloat deltaX = xPosition - lastX;
    GLfloat deltaY = lastY - yPosition;

    //updating last coords
    lastX = xPosition;
    lastY = yPosition;

    camera.mouseIn(deltaX, deltaY);
}

// controlling camera movement
void ScrollCallback(GLFWwindow *window, double deltaX, double deltaY)
{
    camera.mouseScroll(deltaY);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);



}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// controlling camera movement
void KeyMovement()
{
    //simple forwards, backwards and strafe
    //handled by camera object
    if (keys[GLFW_KEY_W])
        camera.keyIn(FORWARD, deltaTime*3);
    if (keys[GLFW_KEY_S])
        camera.keyIn(BACKWARD, deltaTime*3);
    if (keys[GLFW_KEY_A])
        camera.keyIn(LEFT, deltaTime *3);
    if (keys[GLFW_KEY_D])
		camera.keyIn(RIGHT, deltaTime * 3);
    if (keys[GLFW_KEY_SPACE])
        camera.keyIn(UP, deltaTime * 3);
    if (keys[GLFW_KEY_LEFT_CONTROL])
        camera.keyIn(DOWN, deltaTime * 3);
}





//----------------------------------------------------
// Main entry point
// Manages all OpenGL draw calls and main loop
//----------------------------------------------------
int main()
{
	data = davidsmagic();

	//instantiating the window
	//---------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// create the window
	GLFWwindow* window = glfwCreateWindow(windowLength, windowHeight, "Environment Mapping", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Could not create the window." << std::endl;
		glfwTerminate();
		return -1;
	}

	// make context
	glfwMakeContextCurrent(window);

	
	//Gui Set up
	const char* glsl_version = "#version 130";

	if(showGui) {
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGui::StyleColorsDark();
	}

	// associate callbacks
	//--------------------------------------------------------------
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);


	// set the input mode
	//---------------------------------------------------------------
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	// Init GLEW
	//--------------------------------------------------------------
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW not initialized." << std::endl;
		return -1;
	}

	// OpenGL Window and viewport parameter setup
	int widthBuff, heightBuff;
	glfwGetFramebufferSize(window, &widthBuff, &heightBuff);
	glViewport(0, 0, widthBuff, heightBuff);


	//Gui Set up
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Init OpenGL geometry and buffers
	init();
	
		// loop and display as long as the window is open
		while (!glfwWindowShouldClose(window))
		{
			//Gui Set up
			if(showGui){
		    ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			}
			
			//deltaTime is used to calculate camera movement frame independently
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			deltaTime = deltaTime / 3;
			lastFrame = currentFrame;
			processInput(window);
			display(widthBuff, heightBuff);

			if (showGui) {
				//Gui Set Up
				static float f = 0.0f;
				static int counter = 0;

				                          
				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				
				//Gui render
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			}

			// swap buffers
			glfwSwapBuffers(window);

			// io and callbacks
			glfwPollEvents();
			KeyMovement();

		}

		// TO DO - Clean-up your vertex arrays here
		// glDeleteVertexArrays
		//--------------------------------------------
		glDeleteVertexArrays(NumVAOs, VAOs);
		
		//--------------------------------------------

		//Gui Clean up
	if(showGui){
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		}
		// cleanup window
		glfwDestroyWindow(window);

		// end process
	glfwTerminate();

	//Cleanup
	cleanUp();
	// return
	return 0;
}