#pragma once

#include <vector>
#include "alglibmisc.h"


struct KDTreeData {


	//QueryPoints 
	alglib::real_1d_array queryPoint;

	// Nearest Neighbors
	alglib::real_2d_array neighbors;

	//Normals
	alglib::real_1d_array normal;

	//Centroid
	alglib::real_1d_array centroid;

	//Centroids Tags
	alglib::integer_1d_array tagsCentroids;

	//Processed Normal
	alglib::real_1d_array normalOriented;
};

struct OutData
{
	//nPoints
	size_t nPoints = 0;

	//Points
	alglib::real_2d_array points;

	//Radius
	double kRadius;

	//KD Tree Data
	std::vector<KDTreeData> kdTreeData;

	// Riemmanian Graph
	double ** rGraph = nullptr;

	double minX;
	double maxX;
	double minY;
	double maxY;
	double minZ;
	double maxZ;
};