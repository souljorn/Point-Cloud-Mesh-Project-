#pragma once
// included files
#define GLEW_STATIC
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <math.h>
#include "Geometry.h"
#include "colors.h"
#include <set>
#include <algorithm>

//Definition to get error codes from openGL
#define BUFFER_OFFSET(a) ((void*)(a))
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))


//Custom Comparator for set data structure
struct custom_comparator {
	bool operator()(const std::pair<int, int>& a,
		const std::pair<int, int>& b) const
	{
		return less_comparator(std::minmax(a.first, a.second),
			std::minmax(b.first, b.second));
	}

	std::less<std::pair<int, int>> less_comparator;
};
//----------------------------------------------
//		Globals
//----------------------------------------------
enum Axis { Xaxis, Yaxis, Zaxis };
static int delay = 0;
static int graphIndex = 0;
static int graphIndexMan = 0;
int oldDelay;
static int nnIndex = 0;



//----------------------------------------------
//		Error Wrapper
//----------------------------------------------

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error](" << std::hex << error << "): "
			<< function << "Line: " << line << std::endl;
		return false;
	}
	return true;
}


// Program to find modulo of floating  
// point numbers. 
double find_Mod(double a, double b)
{
	// Handling negative values 
	if (a < 0)
		a = -a;
	if (b < 0)
		b = -b;

	// Finding mod by repeated subtraction 
	double mod = a;
	while (mod >= b)
		mod = mod - b;

	// Sign of result typically depends 
	// on sign of a. 
	if (a < 0)
		return -mod;

	return mod;
}
//----------------------------------------------
// Custom Vertex Class
//----------------------------------------------
struct Vertex {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float a;


	//default constructor
	Vertex() {}

	//x,y,z constructor default red 
	Vertex(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = DarkRed.r;
		this->g = DarkRed.g;
		this->b = DarkRed.b;
		this->a = DarkRed.a;
	}
	//Constructor with separate variables for postition and colors
	Vertex(float x, float y, float z, float r, float g, float b, float a) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	//Constructor with float position and color
	Vertex(float x, float y, float z, color color) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = color.r;
		this->g = color.g;
		this->b = color.b;
		this->a = color.a;
	}

	//Constructor with Vertex and Color
	Vertex(Vertex vert, color color) {
		this->x = vert.x;
		this->y = vert.y;
		this->z = vert.z;
		this->r = color.r;
		this->g = color.g;
		this->b = color.b;
		this->a = color.a;
	}

	void flipVertex()
	{
		this->x = -1 * this->x;
		this->y = -1 * this->y;
		this->z = -1 * this->z;
	}

	//Normalize Vectors
	Vertex normalizeVertex(float x, float y, float z) {
		float length = sqrtf(x * x + y * y + z * z);
		Vertex temp;
		temp.x = x / length;
		temp.y = y / length;
		temp.z = z / length;
		return temp;
	}

	Vertex normalizeVertex(Vertex vector) {
		float length = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
		Vertex temp;
		temp.x = vector.x / length;
		temp.y = vector.y / length;
		temp.z = vector.z / length;
		return temp;
	}
};



float distance3D(Vertex p1, Vertex p2)
{
	float a = p1.x - p2.x;
	float b = p1.y - p2.y;
	float c = p1.z - p2.z;

	return abs(sqrt(a * a + b * b + c * c));
}


//----------------------------------------------
// Custom Face Class
//----------------------------------------------
struct Faces {
	int a;
	int b;
	int c;

	Faces() {}

	Faces(int a, int b, int c) {
		this->a = a;
		this->b = b;
		this->c = c;
	}
};


class Mesh {
public:

	//Variables
	std::vector<Vertex> vertices;
	std::vector<Faces> faces;
	std::vector<unsigned int> indices;

	//Mesh Buffers
	GLuint						m_vbo_vertices;
	GLuint						m_vbo_indices;
	GLuint                      m_VAO;
	inline Mesh() {}

	//Mesh creation with vertices and and faces
	Mesh(std::vector<Vertex> vertices, std::vector<Faces> faces) {
		this->vertices = vertices;
		this->faces = faces;
	}

	//Constructor with vertices and indicies
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
		this->vertices = vertices;
		this->faces = faces;
	}

	//Destructor
	~Mesh()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_vbo_indices);
		glDeleteBuffers(1, &m_vbo_vertices);
	}

	//Create General Buffer for lines with indicies
	void createBuffers(GLuint VAO)
	{
		m_VAO = VAO;
		glBindVertexArray(m_VAO);
		glGenBuffers(1, &m_vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, getNumVertices() * 7 * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_vbo_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, getNumIndices() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));

		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(1));

		GLCall(glBindVertexArray(0));
	}

	//Create buffer for points with indices
	void createBuffersPointsGroups(GLuint VAO)
	{
		m_VAO = VAO;
		glBindVertexArray(m_VAO);
		glGenBuffers(1, &m_vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, getNumVertices() * 7 * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_vbo_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, getNumIndices() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));

		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(1));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));
	}

	//Create Buffer for Triangles (i.e. faces) 
	void createBufferTriangle(GLuint VAO)
	{
		m_VAO = VAO;
		glBindVertexArray(m_VAO);
		glGenBuffers(1, &m_vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, getNumVertices() * 7 * sizeof(float), &vertices.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_vbo_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, getNumIndices() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));

		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(1));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));
		
	}

	//Create Buffer that does not need to index points
	void createBufferPoints(GLuint VAO)
	{
		m_VAO = VAO;

		GLCall(glBindVertexArray(m_VAO));
		GLCall(glGenBuffers(1, &m_vbo_vertices));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices));
		GLCall(glBufferData(GL_ARRAY_BUFFER, 7 * getNumVertices() * sizeof(float), &getVertices().front(), GL_STATIC_DRAW));

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));

		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(1));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindVertexArray(0));

	}

	//Draw all lines in the mesh
	void drawLines()
	{

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, (void*)0));
		GLCall(glBindVertexArray(0));
	}


	//Draw all lines in the mesh
	void drawLinesIndexed()
	{

		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)((2 * nnIndex) * sizeof(GLuint))));
		GLCall(glBindVertexArray(0));
	}

	//Draw lines in sequence iterating through all lines in the mesh
	void drawTriangle()
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawElements(GL_TRIANGLES, getNumFaces() * 3, GL_UNSIGNED_INT, (void*)0));
		GLCall(glBindVertexArray(0));
	}

	//Draw lines in sequence iterating through all lines in the mesh
	void drawTriangles()
	{
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawElements(GL_TRIANGLES, getNumIndices(), GL_UNSIGNED_INT, (void*)0));
		GLCall(glBindVertexArray(0));
	}

	//Draw lines in sequence iterating through all lines in the mesh
	void drawLinesSequenceGraph(float time, int modFactor)
	{
		//Rendering sequenced
		graphIndex = static_cast<unsigned int>(find_Mod(time * 200, modFactor));
		//std::cout << delay << std::endl;
		//oldDelay != delay ? delay = delay + 1 : oldDelay = delay;
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawElements(GL_LINES, graphIndex, GL_UNSIGNED_INT, (void*)0));
		GLCall(glBindVertexArray(0));

	}

	


	//Draw all points in the mesh
	void drawPoints()
	{
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawArrays(GL_POINTS, 0, getNumVertices()));
		GLCall(glBindVertexArray(0));
	}

	void drawIndexedPoint()
	{
		nnIndex;
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glDrawArrays(GL_POINTS, nnIndex,1));
		GLCall(glBindVertexArray(0));
	}

	//Draw Points in a mesh in in a sequence
	//time is passed from display funciton
	//modfactor is the size of all the indicies so overflow does not occur, rolls over to the beginning of indices
	//group count is a vector of number of points in each group of nearest neighbors 
	//speed is how fast we animate contoled with the O/P key
	void drawPointGroups(float time, int modFactor, std::vector<unsigned int> groupCount, std::vector<unsigned int> NNstartIndex, int speed)
	{
		//delay = static_cast<unsigned int>(find_Mod(time * speed, groupCount.size() - 1));

		//Prevent over flow
		
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glDrawElements(GL_POINTS, groupCount.at(nnIndex) - 1, GL_UNSIGNED_INT, (void*)( NNstartIndex.at(nnIndex) * sizeof(GLuint))));
		GLCall(glBindVertexArray(0));
		
	}

	void drawPointGroupsSequenced(float time, int modFactor, std::vector<unsigned int> groupCount, std::vector<unsigned int> NNstartIndex, int speed)
	{
		delay = static_cast<unsigned int>(find_Mod(time * speed, groupCount.size()));

		//Prevent over flow
	
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices));
		GLCall(glDrawElements(GL_POINTS, groupCount.at(delay), GL_UNSIGNED_INT, (void*)(NNstartIndex.at(delay) * sizeof(GLuint))));
		GLCall(glBindVertexArray(0));

	}

	//Set indices of faces
	Faces setIndices(int a, int b, int c) {
		Faces temp(a, b, c);
		return temp;
	}

	//Set indices of lines
	void setIndices(int a, int b) {
		indices.push_back(a);
		indices.push_back(b);
	}

	//Creation of a basic square
	void createSquare() {
		vertices.push_back(Vertex(0.1f, 0.0f, 0.0f, Yellow));
		vertices.push_back(Vertex(0.0f, 0.0f, 0.0f, Orange));
		vertices.push_back(Vertex(0.1f, 0.0f, 0.3f, Violet));
		vertices.push_back(Vertex(0.2f, 0.0f, .10f, Teal));

		setIndices(1, 2);
		setIndices(2, 3);
		setIndices(3, 1);
	}

	//Creation of a point cloud mesh from a file default color
	void createPointCloud(std::string filePath) {
		Geometry * pointCloud = new Geometry();
		pointCloud->Load(filePath);
		int size = pointCloud->m_vertices.size();
		float x, y, z;

		for (int i = 0; i < size; i++) {
			x = pointCloud->m_vertices.at(i).data.at(0) / 5;
			y = pointCloud->m_vertices.at(i).data.at(1) / 5;
			z = pointCloud->m_vertices.at(i).data.at(2) / 5;
			vertices.push_back(Vertex(x, y, z,
				MidnightBlue.r, MidnightBlue.g, MidnightBlue.b, MidnightBlue.a));

		}
	}

	//Creates a group of  ranndom lines
	void createRandomLines(int num) {
		srand(time(NULL));
		int j;
		float x, y, z, r, g, b;
		for (int i = 0; i < num; i++) {
			for (j = 0; j < 2; j++) {
				x = (float)(rand() % 10) / 100.0;
				y = (float)(rand() % 10) / 100.0;
				//z = (float)(rand() % 10) / 100.0;
				z = 2.0f;
				r = (float)(rand() % 10) / 100.0;
				g = (float)(rand() % 10) / 100.0;
				b = (float)(rand() % 10) / 100.0;
				//std::cout << x << "," << y << "," << z << std::endl;
				vertices.push_back(Vertex(x, y, z, DarkSlateBlue));
			}
			//Set the indices of the lines
			setIndices(i + j - 2, i + j - 1);
			//std::cout << "(" << (i + j - 2) << "," << (i + j - 1) << ")" << std::endl;
		}
	}

	//Create Lines gives list of origin vertices and direction vector
	void createLines(std::vector<Vertex> origin, std::vector<Vertex> direction, color color,float alpha)
	{
		//vertices.push_back(Vertex(0.0f, 0.0f, 0.0f, BlueViolet));
		for (int i = 0; i < origin.size(); i++) {
			vertices.push_back(Vertex(origin.at(i).x,origin.at(i).y,origin.at(i).z, color.x, color.y, color.z, alpha));
			vertices.push_back(Vertex(direction.at(i).x, direction.at(i).y, direction.at(i).z, color.x, color.y, color.z, alpha));
			setIndices(2*i,2*i+1);
		}
	}

	void createLines(std::vector<Vertex> points, color color, std::set<std::pair<unsigned int, unsigned int>, struct custom_comparator> adjacentSet)
	{

		//Add all points for the mesh
		for (auto point : points)
		{
			vertices.push_back(Vertex(point, color));
		}

		//create an iterator
		std::set<std::pair<unsigned int, unsigned int>>::iterator it;

		for (it = adjacentSet.begin(); it != adjacentSet.end(); ++it)
		{
			setIndices(it->first, it->second);
		}

	}

	//Create Lines gives list of origin vertices and direction vector
	void createLines(std::vector<std::pair<unsigned int, unsigned int>> pointPair, std::vector<Vertex> queryPoints, color color)
	{
		//vertices.push_back(Vertex(0.0f, 0.0f, 0.0f, BlueViolet));
		for (int i = 0; i < pointPair.size() ; i++) {
			if(i == 255)
				std::cout << "";
			if (pointPair.at(i).first > 0 && pointPair.at(i).second > 0) {
				vertices.push_back(Vertex(queryPoints.at(pointPair.at(i).first), color));
				vertices.push_back(Vertex(queryPoints.at(pointPair.at(i).second), color));
				setIndices(pointPair.at(i).first, pointPair.at(i).second);
			}
		}
		std::cout << "";
	}

	//Create points with a vertex list and a color
	void createPoints(std::vector<Vertex> points, color color)
	{
		int i = 0;
		for (auto point : points) {
			vertices.push_back(Vertex(point,color));
		}
	}

	//Create a collection of points with indices *Gives ability to only render portions of points
	//Start and end were considered to be able to distinguish what indices belong to the nn-groups
	//**To do create a pair solution to keep track to start and end indicies of each group
	void createPointsIndices(std::vector<Vertex> &points, GLuint start, GLuint end, color color)
	{
		indices.push_back(getNumVertices());
		for (auto point : points) {
			vertices.push_back(Vertex(point, glm::vec4(color.r, color.g, color.b, 1.0f)));
			indices.push_back(getNumVertices());
		}
		
	}

	void createBoundingBox(float xMin, float xMax,float yMin, float yMax,float zMin, float zMax, color col)
	{
		color temp;
		temp.x = col.x;
		temp.y = col.y;
		temp.x = col.z;
		temp.a = .50f;

		vertices.push_back(Vertex(xMin, yMin, zMin, Red.x, Red.y,Red.z, .50f)); //0
		vertices.push_back(Vertex(xMax, yMin, zMin, Green.x, Green.y, Green.z, .50f)); //1
		vertices.push_back(Vertex(xMax, yMax, zMin, WhiteSmoke.x, WhiteSmoke.y, WhiteSmoke.z, .50f)); //2
		vertices.push_back(Vertex(xMin, yMax, zMin, Orange.x, Orange.y, Orange.z, .50f)); //3
		
		vertices.push_back(Vertex(xMax, yMin, zMax, WhiteSmoke.x, WhiteSmoke.y, WhiteSmoke.z, .50f)); //4
		vertices.push_back(Vertex(xMin, yMin, zMax, WhiteSmoke.x, WhiteSmoke.y, WhiteSmoke.z, .50f)); //5
		vertices.push_back(Vertex(xMin, yMax, zMax, WhiteSmoke.x, WhiteSmoke.y, WhiteSmoke.z, .50f)); //6
		vertices.push_back(Vertex(xMax, yMax, zMax, WhiteSmoke.x, WhiteSmoke.y, WhiteSmoke.z, .50f)); //7

		//indices

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(0);

		indices.push_back(1);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(2);
		indices.push_back(1);

		indices.push_back(4);
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(3);
		indices.push_back(7);
		indices.push_back(4);

		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(1);
		indices.push_back(1);
		indices.push_back(0);
		indices.push_back(4);

		indices.push_back(3);
		indices.push_back(2);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(3);


		//Faces

		//Front
		//faces.push_back(Faces(1, 2, 0));
		//faces.push_back(Faces(3, 0, 2));
		//Back
		/*faces.push_back(Faces(4, 5, 6));
		faces.push_back(Faces(4, 6, 7));*/
		////Left
		//faces.push_back(Faces(1, 4, 7));
		//faces.push_back(Faces(1, 7, 2));
		////Right
		//faces.push_back(Faces(5, 0, 3));
		//faces.push_back(Faces(5, 3, 6));
		////Top
		//faces.push_back(Faces(3, 2, 7));
		//faces.push_back(Faces(3, 7, 6));
		////Bottom
		//faces.push_back(Faces(5, 4, 1));
		//faces.push_back(Faces(5, 1, 0));
	
	}
	//Add points with out indicies
	void addPoints(std::vector<Vertex> points, color color)
	{
		int i = 0;
		for (auto point : points) {
			vertices.push_back(Vertex(point, color));
		}
	}

	//Get distance in one dimension
	float distance1D(float x1, float x2)
	{
		return sqrt(pow((x2 - x1), 2));
	}

	

	//Create an X,Y, or Z aligned 2d grid with a certain amount of divisions, color
	//Left, right and Top, Bottom determine position and size of grid
	void createGrid(int divisions, float l, float r, float b, float t, int axis, color color)
	{
		float offsetH = distance1D(r,l) / divisions;
		float offsetV = distance1D(t,b) / divisions;

		//Set Grid Points
		for (int i = 0; i <= divisions; i++) {
			for (int j = 0; j <= divisions; j++) {

				if (axis == Xaxis)
					vertices.push_back(Vertex(0.0f, l + (i * offsetH), b + (j * offsetV), color));
				else if (axis == Yaxis)
					vertices.push_back(Vertex(l + (i * offsetH), -1.5f, b + (j * offsetV), color));
				else if (axis == Zaxis)
					vertices.push_back(Vertex(l + (i * offsetH), b + (j * offsetV), 0.0f, color));
				//std::cout << "PointAdded:( " << vertices.back().x << " , " << vertices.back().y << " , " << vertices.back().z << " )" << std::endl;
			}

		}
		//Set indices
		for (int i = 0; i < divisions + 1; i++) {
			for (int j = 0; j < divisions; j++)
			{
				setIndices(j + i * (divisions + 1), j + (i * (divisions + 1)) + 1);
			}
		}

		for (int i = 0; i < divisions + 1; i++) {
			for (int j = 0; j < divisions; j++)
			{
				setIndices(i + j * (divisions +1), i + (j + 1) * (divisions+1));
			}
		}
		//Check value of indexes created
		for (auto index : indices)
		{
			//std::cout << "Indices Added:(" << index << ")" << std::endl;
		}
	}

	//Create an Icosphere of verticies and indices
	//Pulled from project 1 
	void createIsoshpere() {
		float pos = (1.0 + sqrtf(5.0)) / 2.0;

		vertices.push_back(normalizeVertices(-1, pos, 0));
		vertices.push_back(normalizeVertices(1, pos, 0));
		vertices.push_back(normalizeVertices(-1, -pos, 0));
		vertices.push_back(normalizeVertices(1, -pos, 0));

		vertices.push_back(normalizeVertices(0, -1, pos));
		vertices.push_back(normalizeVertices(0, 1, pos));
		vertices.push_back(normalizeVertices(0, -1, -pos));
		vertices.push_back(normalizeVertices(0, 1, -pos));

		vertices.push_back(normalizeVertices(pos, 0, -1));
		vertices.push_back(normalizeVertices(pos, 0, 1));
		vertices.push_back(normalizeVertices(-pos, 0, -1));
		vertices.push_back(normalizeVertices(-pos, 0, 1));

		//Top
		faces.push_back(setIndices(1, 0, 5));
		faces.push_back(setIndices(1, 5, 9));
		faces.push_back(setIndices(1, 9, 8));
		faces.push_back(setIndices(1, 8, 7));
		faces.push_back(setIndices(1, 7, 0));

		//Bottom
		faces.push_back(setIndices(2, 11, 4));
		faces.push_back(setIndices(2, 4, 3));
		faces.push_back(setIndices(2, 3, 6));
		faces.push_back(setIndices(2, 6, 10));
		faces.push_back(setIndices(2, 10, 11));

		//Side
		faces.push_back(setIndices(0, 11, 5));
		faces.push_back(setIndices(5, 11, 4));
		faces.push_back(setIndices(5, 4, 9));
		faces.push_back(setIndices(9, 4, 3));
		faces.push_back(setIndices(9, 3, 8));
		faces.push_back(setIndices(8, 3, 6));
		faces.push_back(setIndices(8, 6, 7));
		faces.push_back(setIndices(7, 6, 10));
		faces.push_back(setIndices(0, 10, 11));
		faces.push_back(setIndices(7, 10, 0));

	}

	void createTriangle(Vertex p1,Vertex p2, Vertex p3, color color)
	{
		vertices.push_back(Vertex(p1, glm::vec4(color.r, color.g, color.b, 0.2f)));
		vertices.push_back(Vertex(p2, glm::vec4(color.r, color.g, color.b, 0.2f)));
		vertices.push_back(Vertex(p3, glm::vec4(color.r, color.g, color.b, 0.2f)));

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		faces.push_back(Faces(0, 1, 2));
	}

 

	int getNumVertices() {
		return vertices.size();
	}

	int getNumIndices() {
		return indices.size();
	}
	int getNumFaces() {
		return faces.size();
	}

	std::vector<Vertex> & getVertices() {
		return vertices;
	}

	std::vector<unsigned> & getIndices() {
		return indices;
	}

	std::vector<Faces> & getFaces() {
		return faces;
	}

	//Normalize Vectors
	Vertex normalizeVertices(float x, float y, float z) {
		float length = sqrtf(x * x + y * y + z * z);
		Vertex temp;
		temp.x = x / length;
		temp.y = y / length;
		temp.z = z / length;
		return temp;
	}

	Vertex normalizeVertices(Vertex vector) {
		float length = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
		Vertex temp;
		temp.x = vector.x / length;
		temp.y = vector.y / length;
		temp.z = vector.z / length;
		return temp;
	}

	void flipVectors()
	{	
		for (auto vertex : vertices)
		{
			vertex.x = -1 * vertex.x;
			vertex.y = -1 * vertex.y;
			vertex.z = -1 * vertex.z;
		}
			
	}
};