//------------------------------------------------------------
// Loads and initializes geometry
// NVIDIA teapot
// File Loader: Keith Lantz
//-------------------------------------------------------------


#include "Geometry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include "utilities.h"




// Construct  object
Geometry::Geometry()
{
}


// Destructor for Teapot object
Geometry::~Geometry()
{
	// clean-up
	m_vertices.clear();
	m_texcoords.clear();
	m_normals.clear();
	m_faces.clear();
}


// Load the teapot geometry
void Geometry::Initialize(std::string filename, GLuint VAO)
{
    m_VAO = VAO;
	Load(filename);
	CreateBuffers();
}

//-----------------------------------------
// File Loader: Keith Lantz
//-----------------------------------------
void Geometry::Load(std::string filename)
{
	std::ifstream ifs(filename.c_str(), std::ifstream::in);
	std::string line, key;


	while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
		key = "";
		std::stringstream stringstream(line);
		stringstream >> key >> std::ws;

		if (key == "v") { // vertex
			VertexObj v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.data.push_back(x);
			}
			m_vertices.push_back(v);
		}
		else if (key == "vp") { // parameter
			VertexObj v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.data.push_back(x);
			}
			m_parameters.push_back(v);
		}
		else if (key == "vt") { // texture coordinate
			VertexObj v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.data.push_back(x);
			}
			m_texcoords.push_back(v);
		}
		else if (key == "vn") { // normal
			VertexObj v; float x;
			while (!stringstream.eof()) {
				stringstream >> x >> std::ws;
				v.data.push_back(x);
			}
			v.Normalize();
			m_normals.push_back(v);
		}
		else if (key == "f") { // face
			FaceObj f; int v, t, n;
			while (!stringstream.eof()) {
				stringstream >> v >> std::ws;
				f.vertex.push_back(v - 1);
				if (stringstream.peek() == '/') {
					stringstream.get();
					if (stringstream.peek() == '/') {
						stringstream.get();
						stringstream >> n >> std::ws;
						f.normal.push_back(n - 1);
					}
					else {
						stringstream >> t >> std::ws;
						f.texture.push_back(t - 1);
						if (stringstream.peek() == '/') {
							stringstream.get();
							stringstream >> n >> std::ws;
							f.normal.push_back(n - 1);
						}
					}
				}
			}
			m_faces.push_back(f);
		}
		else {

		}
	}
	ifs.close();
	std::cout << "               Name: " << filename << std::endl;
	std::cout << "           Vertices: " << number_format(m_vertices.size()) << std::endl;
	std::cout << "         Parameters: " << number_format(m_parameters.size()) << std::endl;
	std::cout << "Texture Coordinates: " << number_format(m_texcoords.size()) << std::endl;
	std::cout << "            Normals: " << number_format(m_normals.size()) << std::endl;
	std::cout << "              Faces: " << number_format(m_faces.size()) << std::endl << std::endl;
}


void Geometry::CreateBuffers()
{
	int size = m_faces.size();
	vdata *vertices_ = new vdata[size * 3];
	unsigned int *indices_ = new unsigned int[size * 3];
	for (int j = 0, i = 0; i < size; i++) {
		vertices_[j].x = m_vertices[m_faces[i].vertex[0]].data[0];
		vertices_[j].y = m_vertices[m_faces[i].vertex[0]].data[1];
		vertices_[j].z = m_vertices[m_faces[i].vertex[0]].data[2];
		vertices_[j].nx = m_normals[m_faces[i].normal[0]].data[0];
		vertices_[j].ny = m_normals[m_faces[i].normal[0]].data[1];
		vertices_[j].nz = m_normals[m_faces[i].normal[0]].data[2];
		indices_[j] = j;
		j++;

		vertices_[j].x = m_vertices[m_faces[i].vertex[1]].data[0];
		vertices_[j].y = m_vertices[m_faces[i].vertex[1]].data[1];
		vertices_[j].z = m_vertices[m_faces[i].vertex[1]].data[2];
		vertices_[j].nx = m_normals[m_faces[i].normal[1]].data[0];
		vertices_[j].ny = m_normals[m_faces[i].normal[1]].data[1];
		vertices_[j].nz = m_normals[m_faces[i].normal[1]].data[2];
		indices_[j] = j;
		j++;

		vertices_[j].x = m_vertices[m_faces[i].vertex[2]].data[0];
		vertices_[j].y = m_vertices[m_faces[i].vertex[2]].data[1];
		vertices_[j].z = m_vertices[m_faces[i].vertex[2]].data[2];
		vertices_[j].nx = m_normals[m_faces[i].normal[2]].data[0];
		vertices_[j].ny = m_normals[m_faces[i].normal[2]].data[1];
		vertices_[j].nz = m_normals[m_faces[i].normal[2]].data[2];
		indices_[j] = j;
		j++;
	}

	glGenBuffers(1, &m_vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(vdata), vertices_, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vbo_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * 3 * sizeof(unsigned int), indices_, GL_STATIC_DRAW);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);

    
	delete[] vertices_;
	delete[] indices_;
}




// Render the teapot object
void Geometry::Draw(GLint vertex, GLint normal)
{
    glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(vertex);
	glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(vdata), 0);
	glEnableVertexAttribArray(normal);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(vdata), (char *)NULL + 12);
	glDrawElements(GL_TRIANGLES, m_faces.size() * 3, GL_UNSIGNED_INT, 0);
}



// clean up buffers
void Geometry::DeleteBuffers(void) 
{
	glDeleteBuffers(1, &m_vbo_indices);
	glDeleteBuffers(1, &m_vbo_vertices);
}







