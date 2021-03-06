//-------------------------------------------------------------
// University of Florida
// copyright 2017 Corey Toler-Franklin
// Used with permission by Tim Botelho, David Cabrera, Andrew Chabab 
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
#include "david_magic.h"// david's magic header

//Forward declarations
class Mesh;
double find_Mod(double a, double b);

//----------------------------------------------
//		VAO/VBO/EBO 
//----------------------------------------------

enum VAO_IDs { PointCloud, Lines, Grid, GridLines,
	XYPlane, NormalsUO, QueryVao, NearestNeighborVAO, CentroidVAO,
	NormalsOriented, TriangleTest, BoundingBoxVAO, AdjacentVAO,
	MSTVao,CentroidIndexed, NumVAOs };
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
Mesh * triangleTest;
Mesh * adjacentMesh;
Mesh * boundingBoxMesh;
Mesh * mstMesh;
Mesh * indexedCentroidPoint;


//----------------------------------------------
// Data Globals
//----------------------------------------------

//Shows gui when set to true
bool showGui = true;
//Set Booleans to control what is shown 
bool normBool = false;
bool autoBool = false; 
bool manBool = false;
bool orientNorm = false;
bool normBool1 = false;
bool reset = false;
bool graphStatic = false;
bool graphAnimated = false;
bool nearestNeighborAnimate = false;
bool centroids = false;
bool query = true;
bool nearestNeighborStatic = false;
bool nearestNeighborAnimated = true;

//Other global variables
double scaleNormal;
double scaleConst;
static float rotateX = 0;
static float rotateY = 0;
static float rotateZ = 0;
bool norm = false;	//Norms on off boolean
int speed = 2.0; //Used to speed up the NN-Animation with the O/P key
int index = 0;
double scaleFactor;
float lengthNormals;
int numLines;

//Variable to set point size through out the project
float PointSize = 3.0;

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
std::vector<unsigned int> nearestNeighborGroupStartIndex;
std::set<std::pair<unsigned int, unsigned int>, custom_comparator> setAdjacent;
std::vector<std::pair<unsigned int,unsigned int>> mstGraph;

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//----------------------------------------------
// Perspective settings
//----------------------------------------------
float fov = 50.0f;
float aspect = (float)windowLength / (float)windowHeight;
float nearPlane = .10f;
float farPlane = 50.0f;


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
void display(int windowWidth, int windowHeight, float rotateF,float sliderF, static float rotateX, static float rotateY, static float rotateZ );

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
	delete centroidMesh;
	delete normalsOriented;
	delete triangleTest;
	delete adjacentMesh;
	delete boundingBoxMesh;
	delete mstMesh;
	delete indexedCentroidPoint;
	
}

//Create planes From Normal and point
void createPlaneEq()
{
	Vertex normal(0.0f, 1.0f, 0.0f);
	Vertex pointOnPlane(0.0f, 1.0f, 1.0f);

	double constantNum = normal.x * pointOnPlane.x + normal.y * pointOnPlane.y +
		normal.z + pointOnPlane.z;
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
	
		
	//Get a list of all adjavcent Pairs to create the rGraph
	for(int i =0; i < data.nPoints; i++)
	{
		for (int j = 0; j < data.nPoints; j++)
		{	
			if (data.rGraph[i][j] < DBL_MAX) {
				setAdjacent.insert(std::make_pair(i, j));
			}
		}  
	}

	
	for (int i = 0; i < data.nPoints; i++) {

		//Place all the nearest neighbor groups into data structures
		rows = data.kdTreeData.at(i).neighbors.rows();
		std::vector<Vertex> temp;
		for (int j = 0; j < rows; j++)
		{
			temp.push_back(Vertex(data.kdTreeData.at(i).neighbors[j][0],
				data.kdTreeData.at(i).neighbors[j][1], data.kdTreeData.at(i).neighbors[j][2], Red));
		}
		//Nerest neighbor points and group size
		nearestNeighbor.push_back(temp);
		nearestNeighborCount.push_back(rows);

		if (i == 0) {
			nearestNeighborGroupStartIndex.push_back(0);
		}
		else {
			nearestNeighborGroupStartIndex.push_back(nearestNeighborCount.at(i-1) + nearestNeighborGroupStartIndex.at(i - 1));
		}

		//Get normals Unordered and Oriented 
		norm = data.kdTreeData.at(i).normal.getcontent();
		normOriented = data.kdTreeData.at(i).normalOriented.getcontent();

		normalsUnordered.push_back(Vertex((float)norm[0], (float)norm[1], (float)norm[2], Red));
		normalOriented.push_back(Vertex((float)normOriented[0], (float)normOriented[1], (float)normOriented[2], Blue));

		//Centroid
		centroid = data.kdTreeData.at(i).centroid.getcontent();
		centroidPoints.push_back(Vertex(centroid[0], centroid[1], centroid[2], Orange));

		//CentroidTag
		tag = data.kdTreeData.at(i).tagsCentroids.getcontent();
		tagData.push_back(tag[0]);
	}

	//Create MST Lines(Not woriking)
	int root = data.mstRootIdx;
	for (int i = 0; i < data.nPoints; i++)
	{
		qp = data.kdTreeData.at(i).queryPoint.getcontent();
		queryPoints.push_back(Vertex((float)qp[0], (float)qp[1], (float)qp[2], Violet));
	}

	for (int i = 0; i < data.nPoints; i++)
	{
		if ( i != root ) {
		
				mstGraph.push_back(std::make_pair(i, data.graphMst[i]));
				
				//Vertex p1(centroidPoints.at(i));
				//Vertex p2(centroidPoints.at(data.graphMst[i]));

				//std::cout << "(" << i;
				//std::cout << "," << data.graphMst[i] << ")" <<std::endl;

				/*std::cout << "p1:";
				std::cout << "(" << p1.x;
				std::cout << "," << p1.y;
				std::cout << "," << p1.z;
				std::cout << "," << "p2:";
				std::cout << "(" << p2.x;
				std::cout << "," << p2.y;
				std::cout << "," << p2.z;
				std::cout << ")";
				std::cout << " >> distance " << distance3D(p1, p2) << std::endl;*/
					
		}
	}

	std::cout << "Number of Points: " << data.nPoints << std::endl;
	std::cout << "Number of Query Points: " << queryPoints.size()<< std::endl;
	std::cout << "Number Centroids: " << centroidPoints.size() << std::endl;
	std::cout << "Start Numbers Size: " << nearestNeighborGroupStartIndex.size() << std::endl;
	std::cout << "Group Numbers Size: " << nearestNeighborCount.size() << std::endl;
	std::cout << "Normals Unoriented Size: " << normalsUnordered.size() << std::endl;
	std::cout << "Normals Oriented Size: " << normalOriented.size() << std::endl;
	std::cout << "graphMST: " << mstGraph.size() << std::endl;
	std::cout << "Adjacent Set: " << setAdjacent.size() << std::endl;
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
	glDepthFunc(GL_LESS);
	
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POLYGON_SMOOTH);
	//glCullFace(GL_FRONT);
	glEnable(GL_MULTISAMPLE);
	glPointSize(PointSize);

	//Create a scale factor that takes into consideration the size of the model
	scaleFactor = scaleConst / sqrt(pow((data.minX - data.maxX), 2) + pow((data.minY - data.maxY), 2) + pow((data.minZ - data.maxZ), 2));

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
	
	/*----------------------------------------------
	 *
	 *     Setup up of vertices, indicies, 
	 *     and buffers for the mesh objects
	 *
	//----------------------------------------------*/
	
	pointCloud = new Mesh();
	//pointCloud->createPointCloud("./PointClouds/xy.obj");
	//pointCloud->createBufferPoints(VAOs[PointCloud]);
	
	square = new Mesh();
	//square->createPoints(normalsUnordered, Red);
	//square->createBufferPoints(VAOs[Lines]);

	grid = new Mesh();
	//grid->createGrid(4, .0f, 0.5f, 0.0f, 0.5f, Zaxis, Orange);
	//grid->createBufferPoints(VAOs[Grid]);

	gridLines = new Mesh();
	//gridLines->createGrid(4, .0f, 0.5f, 0.0f, 0.5f, Zaxis, Blue);
	//gridLines->createBuffers(VAOs[GridLines]);

	//xz plane creation
	xzPlane = new Mesh();
	xzPlane->createGrid(16, data.minX, data.maxX, data.minZ, data.maxZ, Yaxis, DarkSlateGray);
	xzPlane->createBuffers(VAOs[XYPlane]);

	//Mst Creation

	std::cout << "Boundaries of Model Min:";
	std::cout << "("<< data.minX;
	std::cout << "," << data.minY;
	std::cout << "," << data.minZ;
	std::cout << "," << std::endl;

	std::cout << "Boundaries of Model Max:";
	std::cout << "(" << data.maxX;
	std::cout << "," << data.maxY;
	std::cout << "," << data.maxZ;
	std::cout << "," << std::endl;
	

	/*----------------------------------------------
	//		UnOriented Normal Mesh
	//----------------------------------------------*/


	//Shift the normals to the centroids
	for (int i = 0; i < normalsUnordered.size(); i++) {
	
		/*std::cout << "Before:";
		std::cout << "(" << vert.x;
		std::cout << "," << vert.y;
		std::cout << "," << vert.z;
		std::cout << "," << std::endl;*/
		
		 normalsUnordered.at(i).x = scaleNormal * normalsUnordered.at(i).x + centroidPoints.at(i).x;
		 normalsUnordered.at(i).y = scaleNormal * normalsUnordered.at(i).y + centroidPoints.at(i).y;
		 normalsUnordered.at(i).z = scaleNormal * normalsUnordered.at(i).z + centroidPoints.at(i).z;
		
		
	/*	std::cout << "After:";
		std::cout << "("<< vert.x;
		std::cout << "," << vert.y;
		std::cout << "," << vert.z;
		std::cout << "," << std::endl;*/
	}
	normalsUO = new Mesh();
	normalsUO->createLines(centroidPoints, normalsUnordered, Red, .5f);
	normalsUO->createBuffers(VAOs[NormalsUO]);

	/*----------------------------------------------
	//		Oriented Normal Mesh
	//----------------------------------------------*/

	normalsOriented = new Mesh();
	//Shift the normals to the centroids
	for (int i = 0; i < normalOriented.size(); i++) {

		normalOriented.at(i).x = scaleNormal * normalOriented.at(i).x + centroidPoints.at(i).x;
		normalOriented.at(i).y = scaleNormal * normalOriented.at(i).y + centroidPoints.at(i).y;
		normalOriented.at(i).z = scaleNormal * normalOriented.at(i).z + centroidPoints.at(i).z;
	}

	normalsOriented->createLines(centroidPoints, normalOriented, DarkBlue,.5f );
	normalsOriented->createBuffers(VAOs[NormalsOriented]);

	/*----------------------------------------------
	//		Query Point Mesh
	//----------------------------------------------*/

	queryPointMesh = new Mesh();
	color tempColor2(Violet.x, Violet.y, Violet.z, .7f);
	queryPointMesh->createPoints(queryPoints,tempColor2);
	queryPointMesh->createBufferPoints(VAOs[QueryVao]);

	/*----------------------------------------------
	//		Nearest Neighbor Mesh
	//----------------------------------------------*/

	nearestNeighborMesh = new Mesh();
	int start = 0;
	int end = 0;
	for (int i = 0; i < nearestNeighbor.size(); i++) {
		if (i == 0) {
			start = 0;
			end = nearestNeighborCount.at(i) - 1;
		}
		else {
			start =nearestNeighborGroupStartIndex.at(i) + 1;
			end = start + nearestNeighborCount.at(i) - 1;
		}
		//printf("count:%d\n", nearestNeighborCount.at(i));
	
		nearestNeighborMesh->createPointsIndices(nearestNeighbor.at(i), start, end, colors.at((i + 20) % 40));
	}
	//printf("count:%d\n", nearestNeighborMesh->vertices.size());
	nearestNeighborMesh->createBuffersPointsGroups(VAOs[NearestNeighborVAO]);

	/*----------------------------------------------
	//		Centroid Mesh
	//----------------------------------------------*/
	
	centroidMesh = new Mesh();
	color tempColor(Orange.x, Orange.y, Orange.z, .5f);
	centroidMesh->createPoints(centroidPoints, tempColor);
	centroidMesh->createBufferPoints(VAOs[CentroidVAO]);

	/*----------------------------------------------
	//		Triangle Mesh
	//----------------------------------------------*/

	//create Triangle Mesh
	triangleTest = new Mesh();
	triangleTest->createTriangle(centroidPoints.at(0), centroidPoints.at(10), centroidPoints.at(20), Yellow);
	triangleTest->createBufferTriangle(VAOs[TriangleTest]);

	/*----------------------------------------------
	//		Centroid Indexed Mesh
	//----------------------------------------------*/

	indexedCentroidPoint = new Mesh();
	color temp(Red.x, Red.y, Red.z, .5f);
	indexedCentroidPoint->createPoints(centroidPoints, temp);
	indexedCentroidPoint->createBufferPoints(VAOs[CentroidIndexed]);

	/*----------------------------------------------
	//		Bounding Box Mesh
	//----------------------------------------------*/

	boundingBoxMesh = new Mesh();
	boundingBoxMesh->createBoundingBox(scaleFactor *data.minX, scaleFactor * data.maxX, scaleFactor *data.minY, scaleFactor* data.maxY, scaleFactor *data.minZ, scaleFactor * data.maxZ, Teal);
	boundingBoxMesh->createBuffers(VAOs[BoundingBoxVAO]);
	std::cout << "Scale Factor:" << scaleFactor << std::endl;

	/*----------------------------------------------
	//		Adjacency Graph Mesh
	//----------------------------------------------*/

	adjacentMesh = new Mesh();
	adjacentMesh->createLines(queryPoints, DarkGreen, setAdjacent);
	adjacentMesh->createBuffers(VAOs[AdjacentVAO]);

	/*----------------------------------------------
	//		Mst Mesh
	//----------------------------------------------*/
	mstMesh = new Mesh();
	mstMesh->createLines(mstGraph, centroidPoints, Sienna);
	mstMesh->createBuffers(VAOs[MSTVao]);
}	

//----------------------------------------------
// display function
//-----------------------------------------------
void display(int windowWidth, int windowHeight,float rotateF,float sliderF,float manNN, static float rotateX, static float rotateY, static float rotateZ)
{	
	//Camera variables (not used currently)
    float ratio = (float)windowHeight/windowWidth;
	float degrees = (int)(80 * glfwGetTime()) % 360;

	//Time for dynamic rendering/animation
	static float time;

	//if (!manBool) {
		time = glfwGetTime();
	/*}
	else {
		time = manNN;
	}*/
	
	//Clear the buffer
	GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	
	//-------------------------
	//     Matrix Update
	//------------------------
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
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
	modelGrid = glm::scale(modelGrid, glm::vec3(scaleFactor  *  .02, scaleFactor * .02, scaleFactor * .02));
	modelGrid = glm::translate(modelGrid, glm::vec3(0, 0, 0));
	//modelGrid = glm::rotate(modelGrid, rotateF * 5, glm::vec3(1.0, 0.0f, 0.1f));


	modelGrid = glm::rotate(modelGrid, rotateX * 5, glm::vec3(1.0f, 0.0f, 0.0f));

	modelGrid = glm::rotate(modelGrid, rotateY * 5, glm::vec3(0.0f, 1.0f, 0.0f));

	modelGrid = glm::rotate(modelGrid, rotateZ * 5, glm::vec3(0.0f, 0.0f, 1.0f));

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 modelPoint = glm::mat4(1.0f);
	modelPoint = glm::scale(modelPoint, glm::vec3(scaleFactor  *  .02, scaleFactor * .02, scaleFactor * .02));
	modelPoint = glm::translate(modelPoint, glm::vec3(0, 0, 0));
	modelPoint = glm::rotate(modelPoint, rotateF * 5 , glm::vec3(0, 1.0f, 0.1f));

	//-------------------------------------------------------
	//     Activate Shader
	//-------------------------------------------------------
	
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
	
	/*
	if(progressBool){
		// draw neighbor mesh with time 
	} else {
		// draw with slider
	}
	*/
	
	
	//if(nearestNeighborAnimate)
	//nearestNeighborMesh->drawPointGroups(time, nearestNeighborMesh->indices.size(), nearestNeighborCount, nearestNeighborGroupStartIndex, speed);
	//pointCloud->drawPoints();
	
	//gridLines->drawLines(0, 0, 0);

	//Transparent Objects must be drawn last
	//triangleTest->drawTriangle();
	
	if (graphAnimated) {
		
		adjacentMesh->drawLinesSequenceGraph(time, adjacentMesh->getNumIndices());
	}
	if (graphStatic) {
		adjacentMesh->drawLines();
	}
	
	///mstMesh->drawLinesSequenceGraph(time,mstMesh->getNumVertices(),mstGraph);

	//indexedCentroidPoint->drawIndexedPoint();
	//---------Transparent Objects----------------
	if (centroids) {
		centroidMesh->drawPoints();
	}
	
	if (norm) {
		normalsUO->drawLines();
	}
	if (orientNorm) {
		normalsOriented->drawLines();
	}
	if (nearestNeighborStatic) {
		nearestNeighborMesh->drawPointGroups(time, nearestNeighborMesh->indices.size(), nearestNeighborCount, nearestNeighborGroupStartIndex, speed);
	
	}

	if (nearestNeighborAnimate) {
		nearestNeighborMesh->drawPointGroupsSequenced(time, nearestNeighborMesh->indices.size(), nearestNeighborCount, nearestNeighborGroupStartIndex, speed);
		
	}
	if (query) {
		queryPointMesh->drawPoints();
	}
	//---------------Link Matrices to Point Shader--------------------------------
	pointShaderProgram->Use();
	GLCall(glUniformMatrix4fv(modelIDPoint, 1, GL_FALSE, glm::value_ptr(modelPoint)));
	GLCall(glUniformMatrix4fv(viewIDPoint, 1, GL_FALSE, glm::value_ptr(view)));
	GLCall(glUniformMatrix4fv(projectionIDPoint, 1, GL_FALSE, glm::value_ptr(projection)));
	//Alpha is a float in the shader used to add jitter to the query points
	if (!manBool) {
		GLCall(glUniform1f(alphaBasic, ((sin(time * 150.0f) + 1) * .0005) + .995f));
	}
	else {
		GLCall(glUniform1f(alphaBasic, ((sin(manNN * 150.0f) + 1) * .0005) + .995f));
	}
	//----------Mesh Draw Calls for The Points------------------------------------
	

	
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
	if (key == GLFW_KEY_G && act == GLFW_PRESS)
	{
		graphStatic = !graphStatic;
	}
	if (key == GLFW_KEY_H && act == GLFW_PRESS)
	{
		graphAnimated = !graphAnimated;
	}
	if (key == GLFW_KEY_C && act == GLFW_PRESS)
	{
		graphIndexMan++;
	}
	if (key == GLFW_KEY_V && act == GLFW_PRESS)
	{
		graphIndexMan--;
	}

	if (key == GLFW_KEY_Z && act == GLFW_PRESS)
	{
		nnIndex--;
	}
	if (key == GLFW_KEY_X && act == GLFW_PRESS)
	{
		nnIndex++;
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
	
	globals::radius = .40f;
	globals::cubeEdge = 10.0f;
	globals::filename = "CatPoints.obj";
	globals::outputFilename = "outputMesh.obj";
	scaleNormal = .60f;
	scaleConst = 20.0f;
	

	bool result = do_magic(data); // here is where the magic happens!

	if (!result)
		return 1;
	
	//instantiating the window
	//---------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
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
			//display(widthBuff, heightBuff);

			if (showGui) {
				//Gui Set Up
				static float f = 0.0f;
				static float sliderF = 0.0f;
				static float sliderS = 0.0f; 
				static float manNN = 0.0f;
				static int counter = 0;
				

				                          
				ImGui::Text("Slider for Pitch.");
				ImGui::SliderFloat("5", &rotateX, 0.0f, 100.0f);

				ImGui::Text("Slider for Yaw.");
				ImGui::SliderFloat("6", &rotateY, 0.0f, 100.0f);

				ImGui::Text("Slider for Roll.");
				ImGui::SliderFloat("7", &rotateZ, 0.0f, 100.0f);
				
				

				if (ImGui::Button("Toggle Query Point"))
					query = !query;
				if (ImGui::Button("Toggle Centroid Points"))
					centroids = !centroids;
				if (ImGui::Button("Nearest Neighbor Groups Static"))
					nearestNeighborStatic = !nearestNeighborStatic;
				ImGui::Text("Slider for manual nearest neighbors progression");
				ImGui::SliderFloat("4", &manNN, 0.0f, queryPoints.size() - 1);
				nnIndex = static_cast<unsigned int>(manNN);

				if (ImGui::Button("Nearest Neighbor Groups Animated"))
					nearestNeighborAnimate = !nearestNeighborAnimate;
				if (ImGui::Button("Speed of Animation (+)")) {
					speed += 1;
				} ImGui::SameLine();
				if (ImGui::Button("Speed of Animation (-)")) {
					speed -= 1;
				}
				if (speed < 0) speed = 1;

				if (ImGui::Button("Un-oriented normals on/off"))
					normBool = true;
				// gotta reset the bool right after the norm is set or else it will spaz out 
				if (normBool) {
					norm = !norm;
					normBool = !normBool;
				}

				if (ImGui::Button("Oriented normals on/off"))
					normBool1 = true;
				// gotta reset the bool right after the norm is set or else it will spaz out 
				if (normBool1) {
					orientNorm = !orientNorm;
					normBool1 = !normBool1;
				}
				/*if (ImGui::Button("Nearest Neighbor Animation"))
					nearestNeighborAnimated = !nearestNeighborAnimated;*/
				if (ImGui::Button("Toggle Static Graph"))
					graphStatic = !graphStatic;
				if (ImGui::Button("Toggle Animated Graph"))
					graphAnimated = !graphAnimated;
				
				// zooming in/out 
				if (ImGui::Button("Zoom In"))
					camZ -= 1.1; 
				ImGui::SameLine();
				if (ImGui::Button("Zoom Out"))
					camZ += 1.1;
				// rotating in x and y 
				if (ImGui::Button("Rotate left"))
					camX -= 1.1;
				ImGui::SameLine();
				if (ImGui::Button("Rotate right"))
					camX += 1.1;
				if (ImGui::Button("Pitch up"))
					camY -= 1.1;
				ImGui::SameLine();
				if (ImGui::Button("Pitch down"))
					camY += 1.1;

				// reset button 
				if (ImGui::Button("Reset camera")) {
					camX = -0.8f;
					camY = -2.8f;
					camZ = 30.0f;
					f = 0.0f;
					lookX = 0.024f;
					lookY = 0.049f;
					lookZ = -0.065f;
				}
					
				// program data 
				ImGui::Text("Name of Point Cloud: %s", globals::filename);
				ImGui::Text("Amount of points currently being loaded: %d",data.nPoints);
				if (nearestNeighborStatic && !nearestNeighborAnimate) {
					ImGui::Text("Current query point : (%f,%f,%f)", queryPoints.at(nnIndex).x, queryPoints.at(nnIndex).y, queryPoints.at(nnIndex).z);
					ImGui::Text("Current centroid point:(%f,%f,%f)", centroidPoints.at(nnIndex).x, centroidPoints.at(nnIndex).y, centroidPoints.at(nnIndex).z);
					ImGui::Text("Current neighborhood being operating on: %d", nnIndex);

					int NNcount = nearestNeighborCount.at(nnIndex);
					ImGui::Text("Amount of neighbors that the %d point has: %d", nnIndex, NNcount);
					std::vector<Vertex> nnlist = nearestNeighbor.at(nnIndex);

					ImGui::Text("The nearest neighbors of the current point:");
					for (int i = 0; i < nnlist.size(); i++) {
						//ImGui::Text("The current point's current nearest neighbor is at:");
						ImGui::Text("Index(%d): ", i); ImGui::SameLine();
						ImGui::Text("(%f, %f, %f)", nnlist.at(i).x, nnlist.at(i).y, nnlist.at(i).z);
						//if (i%3!=0 && i>0)
							//ImGui::SameLine();

					}
				}
				else {
					ImGui::Text("Current query point : (%f,%f,%f)", queryPoints.at(delay).x, queryPoints.at(delay).y, queryPoints.at(delay).z);
					ImGui::Text("Current centroid point:(%f,%f,%f)", centroidPoints.at(delay).x, centroidPoints.at(delay).y, centroidPoints.at(delay).z);
					ImGui::Text("Current neighborhood being operating on: %d", delay);

					int NNcount = nearestNeighborCount.at(delay);
					ImGui::Text("Amount of neighbors that the %d point has: %d", delay, NNcount);
					std::vector<Vertex> nnlist = nearestNeighbor.at(delay);

					ImGui::Text("The nearest neighbors of the current point:");
					for (int i = 0; i < nnlist.size(); i++) {
						//ImGui::Text("The current point's current nearest neighbor is at:");
						ImGui::Text("Index(%d): ", i); ImGui::SameLine();
						ImGui::Text("(%f, %f, %f)", nnlist.at(i).x, nnlist.at(i).y, nnlist.at(i).z);
						//if (i%3!=0 && i>0)
							//ImGui::SameLine();

					}
				}

				//camera data 
				ImGui::Text("Camera X data: %f", camX);
				ImGui::Text("Camera Y data: %f", camY);
				ImGui::Text("Camera Z data: %f", camZ);
				
				// display 
				display(widthBuff, heightBuff, f, sliderF,manNN,rotateX,rotateY,rotateZ);
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
