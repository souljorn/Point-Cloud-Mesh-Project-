//------------------------------------------------------------
// Copyright 2015 Corey Toler-Franklin
// Vertex class
// Stores x, y, z coordinates and nx, ny, nx normal vector
//-------------------------------------------------------------


#include "VertexObj.h"
#include <math.h>


VertexObj::VertexObj()
{

}


VertexObj::~VertexObj()
{

}


void VertexObj::Normalize(void)
{
	float magnitude = 0.0f;

	for (int i = 0; i < (int)data.size(); i++)
		magnitude += pow(data[i], 2.0f);

	magnitude = sqrt(magnitude);

	for (int i = 0; i < (int)data.size(); i++)
		data[i] = data[i]/magnitude;
}






