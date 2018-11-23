///////////////////////////////////////////////////////////////////////////////////
// copyright 2015 Corey Toler-Franklin, University of Florida
// Vertex object
// Stores x, y, z coordinates and nx, ny, nx normal vector
//
//////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <GL/glew.h>



// vertex data
struct vdata {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
	GLfloat padding[2];
};


// face
struct FaceObj {
	std::vector<int> vertex;
	std::vector<int> texture;
	std::vector<int> normal;
};


class VertexObj
{

public:

						VertexObj							(void);
						~VertexObj							(void);

	void				Normalize							(void);

	std::vector<float>	data;
};



