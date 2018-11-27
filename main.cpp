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
#include "davidmagic.h"// david's magic header

/***********To Do************************
 *Animate NN by point groups
 *Animate normals by Centroid
 *
 *
 *
 ****************************************/

class Mesh;
double find_Mod(double a, double b);
//----------------------------------------------
//		VAO/VBO/EBO 
//----------------------------------------------

enum VAO_IDs { PointCloud, Lines, Grid, GridLines,
	XYPlane, NormalsUO, QueryVao, NearestNeighborVAO, CentroidVAO, NormalsOriented, NumVAOs };
GLuint  VAOs[NumVAOs];

//Shader pointers
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
Mesh * centroidMesh;
Mesh * normalsOriented;

//----------------------------------------------
// Data Globals
//----------------------------------------------

//Shows gui when set to true
bool showGui = false;

//Variable to set point size through out the project
float PointSize = 3.0;
int numLines;

//Data object from algorithm header
OutData data;

//Data structures to hold all the data from algorithm
std::vector<Vertex> normalsUnordered;
std::vector<Vertex> normalOriented;
std::vector<Vertex> queryPoints; 
std::vector<std::vector<Vertex>> nearestNeighbor;
std::vector<Vertex> centroidPoints;
std::vector<int> tagData;
std::vector<unsigned int> nearestNeighborCount;
std::vector<unsigned int> NNGroupStartIndex;

bool norm = false;	//Norms on off boolean
int speed = 8; //Used to speed up the NN-Animation with the O/P key
float nearPlane = .10f;
float farPlane = 50.0f;

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
	delete normalsUO;
	delete queryPointMesh;
	delete nearestNeighborMesh;
	//delete nearestNeighborMesh1;
	delete centroidMesh;
	
}

//Parse Data from algorithm header

void parseData()
{
	double *norm;
	double * qp;
	double * centroid;
	int * tag;
	int rows;
	int i= 0;
	double * normOriented;
	//Push the first index then push i-1 + row
	NNGroupStartIndex.push_back(0);
	for (auto d : data.kdTreeData) {
		
	/*	std::cout << "For query point " << d.queryPoint.tostring(constants::psd) << " with kRadius " << data.kRadius << std::endl;
		std::cout << "The neighborhood is the set " << d.neighbors.tostring(constants::psd) << std::endl;
		std::cout << "The centroid is " << d.centroid.tostring(constants::psd) << std::endl;
		std::cout << "And the normal is " << d.normal.tostring(constants::psd) << std::endl;
		std::cout << "With tag index " << d.tagsCentroids[i] << std::endl << std::endl;
		i++;*/
	}

	for (int i = 0; i < data.nPoints ; i++) {
		
		//Nearest Neighbor
		rows = data.kdTreeData.at(i).neighbors.rows();
		std::vector<Vertex> temp;
		for(int j = 0; j < rows; j++)
		{
			temp.push_back(Vertex(data.kdTreeData.at(i).neighbors[j][0],
				data.kdTreeData.at(i).neighbors[j][1], data.kdTreeData.at(i).neighbors[j][2], Red));			
		}
		//Nerest neighbor points and group size
		nearestNeighbor.push_back(temp);
		nearestNeighborCount.push_back(rows);
		NNGroupStartIndex.push_back(rows + NNGroupStartIndex.at(i));
		//std::cout << "Start Numbers::" << NNGroupStartIndex.back() << std::endl;
		//Get norms
		norm = data.kdTreeData.at(i).normal.getcontent();
		normOriented = data.kdTreeData.at(i).normalOriented.getcontent();
		
	
		normalsUnordered.push_back(Vertex((float)norm[0] , (float)norm[1], (float)norm[2] , Violet));
		normalOriented.push_back(Vertex((float)normOriented[0], (float)normOriented[1], (float)normOriented[2], Green));
		//Query Points
		qp = data.kdTreeData.at(i).queryPoint.getcontent();
		queryPoints.push_back(Vertex((float)qp[0], (float)qp[1], (float)qp[2], Violet));

		//Centroid
		centroid = data.kdTreeData.at(i).centroid.getcontent();
		centroidPoints.push_back(Vertex(centroid[0], centroid[1], centroid[2], Orange));

		//CentroidTag
		tag = data.kdTreeData.at(i).tagsCentroids.getcontent();
		tagData.push_back(tag[0]);
	}
	//Remove the end index
	NNGroupStartIndex.pop_back();
	std::cout << "Start Numbers Size::" << NNGroupStartIndex.size() << std::endl;
	std::cout << "Group Numbers Size::" << nearestNeighborCount.size() << std::endl;
}

//----------------------------------------------
// init
//----------------------------------------------
void init()
{
	//Generate all the VAOS
	GLCall(glGenVertexArrays(NumVAOs, VAOs));
	
	//Create Colors Vector to be used to easily cycle through
	//all the colors in colors.h
	create_colors();

	//Parse Data from Algorithm header file(davidmagic.h)
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

	// Setup up of vertices, indicies, and buffers for the mesh objects
	// ------------------------------------------------------------------
	std::cout << "NN-Count:" << nearestNeighborCount[0] << std::endl;
	//Create Point Cloud from file
	pointCloud = new Mesh();
	//pointCloud->createPointCloud("./PointClouds/xy.obj");
	//pointCloud->createBufferPoints(VAOs[PointCloud]);

	
	square = new Mesh();
	//square->createPoints(normalsUnordered, Red);
	//square->createBufferPoints(VAOs[Lines]);

	grid = new Mesh();
	grid->createGrid(4, .0f, 0.5f, 0.0f, 0.5f, Zaxis, Orange);
	grid->createBufferPoints(VAOs[Grid]);

	gridLines = new Mesh();
	gridLines->createGrid(4, .0f, 0.5f, 0.0f, 0.5f, Zaxis, Blue);
	gridLines->createBuffers(VAOs[GridLines]);

	//xz plane creation
	xzPlane = new Mesh();
	xzPlane->createGrid(16, -2.0f, 2.0f, -2.0f, 2.0f, Yaxis, DarkSlateGray);
	xzPlane->createBuffers(VAOs[XYPlane]);

	//Normal Mesh
	normalsUO = new Mesh();

	//Shift the normals to the centroids
	for (int i = 0; i < normalsUnordered.size(); i++) {
	
		/*std::cout << "Before:";
		std::cout << "(" << vert.x;
		std::cout << "," << vert.y;
		std::cout << "," << vert.z;
		std::cout << "," << std::endl;*/
		
		 normalsUnordered.at(i).x = 10* normalsUnordered.at(i).x + centroidPoints.at(i).x;
		 normalsUnordered.at(i).y = 10 * normalsUnordered.at(i).y + centroidPoints.at(i).y;
		 normalsUnordered.at(i).z = 10 * normalsUnordered.at(i).z + centroidPoints.at(i).z;
		
		
	/*	std::cout << "After:";
		std::cout << "("<< vert.x;
		std::cout << "," << vert.y;
		std::cout << "," << vert.z;
		std::cout << "," << std::endl;*/
	}

	normalsUO->createLines(centroidPoints, normalsUnordered);
	normalsUO->createBuffers(VAOs[NormalsUO]);

	normalsOriented = new Mesh();
	//Shift the normals to the centroids
	for (int i = 0; i < normalOriented.size(); i++) {

		normalOriented.at(i).x = 10 * normalOriented.at(i).x + centroidPoints.at(i).x;
		normalOriented.at(i).y = 10 * normalOriented.at(i).y + centroidPoints.at(i).y;
		normalOriented.at(i).z = 10 * normalOriented.at(i).z + centroidPoints.at(i).z;
	}

	normalsOriented->createLines(centroidPoints, normalOriented);
	normalsOriented->createBuffers(VAOs[NormalsOriented]);

	//Query Point Mesh
	queryPointMesh = new Mesh();
	queryPointMesh->createPoints(queryPoints, GhostWhite);
	queryPointMesh->createBufferPoints(VAOs[QueryVao]);

	//Nearest Neighbor Mesh
	nearestNeighborMesh = new Mesh();
	int start = 0;
	int end = 0;
	for (int i = 0; i < nearestNeighbor.size(); i++) {
		if (i == 0) {
			start = 0;
			end = nearestNeighborCount.at(i) - 1;
		}
		else {
			start = end + 1;
			end = start + nearestNeighborCount.at(i);
		}
		//printf("count:%d\n", nearestNeighborCount.at(i));
		nearestNeighborMesh->createPointsIndices(nearestNeighbor.at(i), start, end, colors.at((i + 40) % 10));
	}
	//printf("count:%d\n", nearestNeighborMesh->vertices.size());
	nearestNeighborMesh->createBuffersPointsGroups(VAOs[NearestNeighborVAO]);

	//Create Centroid Mesh
	centroidMesh = new Mesh();
	centroidMesh->createPoints(centroidPoints, Orange);
	centroidMesh->createBufferPoints(VAOs[CentroidVAO]);

}

//----------------------------------------------
// display function
//-----------------------------------------------
void display(int windowWidth, int windowHeight)
{	
	//Camera variables (not used currently)
    float ratio = (float)windowHeight/windowWidth;
	float degrees = (int)(80 * glfwGetTime()) % 360;

	//Time for dynamic rendering/animation
	static float time;
	time = glfwGetTime();
	 
	//Clear the buffer
	GLCall(glClearColor(SlateGray.x, SlateGray.y, SlateGray.z, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	
	//-------------------------
	//     Matrix Update
	//------------------------
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowLength) / static_cast<float>(windowHeight), nearPlane, farPlane);

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
	modelGrid = glm::scale(modelGrid, glm::vec3(.002, .002, .002));
	modelGrid = glm::translate(modelGrid, glm::vec3(0, 0, 0));
	modelGrid = glm::rotate(modelGrid, time * 5, glm::vec3(0, 1.0f, 0.1f));

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 modelPoint = glm::mat4(1.0f);
	modelPoint = glm::scale(modelPoint, glm::vec3(.002, .002, .002));
	modelPoint = glm::translate(modelPoint, glm::vec3(0, 0, 0));
	modelPoint = glm::rotate(modelPoint, time *5 , glm::vec3(0, 1.0f, 0.1f));

	//-------------------------
	//     Activate Shader
	//------------------------
	
	basicShaderProgram->Use();
	//---------------Link Matrices to Basic Shader--------------------------------
	GLCall(glUniformMatrix4fv(modelIDBasic, 1, GL_FALSE, glm::value_ptr(modelGrid)));
	GLCall(glUniformMatrix4fv(viewIDBasic, 1, GL_FALSE, glm::value_ptr(view)));
	GLCall(glUniformMatrix4fv(projectionIDBasic, 1, GL_FALSE, glm::value_ptr(projection)));
	//GLCall(glUniform1f(alphaBasic, sin(time * 2.0f) * 0.6f));

	//----------Mesh Draw Calls for The XY PLANE/NEAREST NEIGHBOR/CENTROID/NORMALS------------------------------------
	//gridLines->drawLinesSequence(time, gridLines->getNumIndices() + 1);
	//grid->drawPoints();
	//xzPlane->drawLines(0, 0, 0);
	//Turn Normals on/off with N key
	if (norm) {
		normalsUO->drawLines(0, 0, 0);
	}
	normalsOriented->drawLines(0,0,0);
	nearestNeighborMesh->drawPointGroups(time, nearestNeighborMesh->indices.size(), nearestNeighborCount, NNGroupStartIndex, speed);
	centroidMesh->drawPoints();
	
	

	//---------------Link Matrices to Point Shader--------------------------------
	pointShaderProgram->Use();
	GLCall(glUniformMatrix4fv(modelIDPoint, 1, GL_FALSE, glm::value_ptr(modelPoint)));
	GLCall(glUniformMatrix4fv(viewIDPoint, 1, GL_FALSE, glm::value_ptr(view)));
	GLCall(glUniformMatrix4fv(projectionIDPoint, 1, GL_FALSE, glm::value_ptr(projection)));
	//Alpha is a float in the shader used to add jitter to the query points
	GLCall(glUniform1f(alphaBasic, ((sin(time * 150.0f) + 1) * .0005) + .995f));

	//----------Mesh Draw Calls for The Points------------------------------------
	//pointCloud->drawPoints();
	queryPointMesh->drawPoints();
	//gridLines->drawLines(0, 0, 0);

	//Unbind the VAO
	GLCall(glBindVertexArray(0));
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
		camX +=1.1;
		std::cout << "camX:" << camX << std::endl;
    }

    if (key == GLFW_KEY_DOWN && act == GLFW_PRESS)
    {
		camX -= 1.1;
		std::cout << "camX:" << camX << std::endl;
    }

	if (key == GLFW_KEY_LEFT && act == GLFW_PRESS)
	{
		camY += 1.1;
		std::cout << "camY:" << camY << std::endl;
	}
    if (key == GLFW_KEY_RIGHT && act == GLFW_PRESS)
    {
		camY -= 1.1;
		std::cout << "camY:" << camY << std::endl;
    }

	if (key == GLFW_KEY_1 && act == GLFW_PRESS)
	{
		camZ += 1.1;
		std::cout << "camZ:" << camZ << std::endl;
	}

	if (key == GLFW_KEY_2 && act == GLFW_PRESS)
	{
		camZ -= 1.1;
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
	if (key == GLFW_KEY_O && act == GLFW_PRESS)
	{
		speed -= 1; 
		std::cout << "Speed:" << speed << std::endl;
	}
	if (key == GLFW_KEY_P && act == GLFW_PRESS)
	{
		speed += 1;
		std::cout << "Speed:" << speed << std::endl;
	}
	if (key == GLFW_KEY_Q && act == GLFW_PRESS)
	{
		nearPlane -= .50f;
		std::cout << "NearPlane:" << nearPlane << std::endl;
	}
	if (key == GLFW_KEY_W && act == GLFW_PRESS)
	{
		nearPlane += .50f;
		std::cout << "NearPlane:" << nearPlane << std::endl;
	}
	if (key == GLFW_KEY_A && act == GLFW_PRESS)
	{
		farPlane -= 5.0f;
		std::cout << "FarPlane:" << farPlane << std::endl;
	}
	if (key == GLFW_KEY_S && act == GLFW_PRESS)
	{
		farPlane += 5.0f;
		std::cout << "FarPlane:" << farPlane << std::endl;
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