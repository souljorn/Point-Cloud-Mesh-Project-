#pragma once

// adjust verbosity for debugging
//#define VERY_VERBOSE
//#define DISPLAY_POINT_PLANES
//#define OUTPUT_TO_FILE
//#define PRINT_MST
//#define SHOW_NORMALS


#include <fstream>
#include <iomanip>
#include <windows.h>
#include <set>
#include <stdlib.h>

// alglib nearest neighbor subpackage for kdtree
#include "../alglib/alglibmisc.h";
#include "../alglib/dataanalysis.h";
#include "../alglib/ap.h"


// David's toolbox
#include "david_operations.h"
#include "david_cloudfile.h"
#include "david_constants.h"
#include "david_kdtmanip.h"
#include "david_graph.h"
#include "david_cubes2.h"

#include "libraries/meshgen/MeshReconstruction.h"
#include "libraries/meshgen/IO.h"

// Tim's data collection structure
#include "tim_outdata.h"

// tiny object loader
#define TINYOBJLOADER_IMPLEMENTATIO
#include "../tinyobj/tiny_obj_loader.h";





bool do_magic(OutData& outData)
{

#ifdef OUTPUT_TO_FILE
	std::ofstream out("./out.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

	/*
	*	LOAD THE OBJ FILE
	*/

	//std::string filename = cloudfile::getCloudPointFilename();
	std::string filename = constants::cloudPointsBasePath + "CatPoints.obj";
	std::cout << "Loading " << filename << " wavefront file..." << std::endl;

	tinyobj::attrib_t pcloud;
	bool loaded = cloudfile::loadCloud(pcloud, filename);

	if (!loaded) {
		std::cerr << "ERROR: The OBJ file could not be loaded!" << std::endl;
		return false;
	}


	/*
	*	CAST DATA POINTS
	*	TO DOUBLE FOR ALGLIB
	*
	*		cast data points to double and init real_2d_array for alglib kdtree
	*
	*/

	size_t nPoints = pcloud.vertices.size() / constants::dims;

	std::cout << "Casting and adapting format on " << nPoints << " points..." << std::endl;

	alglib::real_2d_array points;
	cloudfile::adaptDataPoints(pcloud.vertices, nPoints, points);

	// struct to export data points
	outData.nPoints = nPoints;
	outData.points = points;


	/*
	*	ESTIMATE PLANES
	*
	*		1. build kdtree with data points
	*		2. estimate planes
	*			a. obtain centroids
	*			b. do PCA to obtain normals
	*
	*/

	double kRadius = .45f;	// this should be a function of 
							// the density and noise of the point cloud
	globals::radius = &kRadius;
	outData.kRadius = kRadius;

	std::cout << "Building an " << nPoints << " points kdtree using norm2 (euclidean)..." << std::endl;

	// build kdtree
	alglib::kdtree kdt;
	kdtmanip::buildKDTree(kdt, points, nPoints);

	// estimate planes
	std::cout << "Estimating planes under a neighborhood radius of " << kRadius << "..." << std::endl;

	// indexed normals
	alglib::real_2d_array normals;
	globals::normals = &normals; // keep a reference for the signed distance
	normals.setlength(nPoints, constants::dims);

	// indexed centroids
	alglib::real_2d_array centroids;
	globals::centroids = &centroids;
	centroids.setlength(nPoints, constants::dims);

	// tags to centroid index
	alglib::integer_1d_array tagsCentroids;
	tagsCentroids.setlength(nPoints);

	// for every point
	for (size_t i = 0; i < nPoints; i++)
	{
		//Temp struct to add to Outdata for data export
		KDTreeData temp;

		// generate the query point array
		alglib::real_1d_array queryPoint = ops::makeArr(points[i], constants::dims);

		// get the k neighbors of the point for a given radius
		alglib::real_2d_array neighbors;
		alglib::ae_int_t k;
		k = kdtmanip::getKNeighbors(kdt, neighbors, queryPoint, kRadius);

		// calculate centroid
		alglib::real_1d_array centroid = ops::getCentroid(neighbors);
		ops::copyArr(centroids[i], centroid);

		// keep centroids tag indexes to retrieve normals later
		tagsCentroids[i] = i;

		// calculate normal
		alglib::real_1d_array normal = ops::getNormal(neighbors);
		ops::copyArr(normals[i], normal);

		// tim's data
		temp.queryPoint = queryPoint;
		temp.neighbors = neighbors;
		temp.centroid = centroid;
		temp.tagsCentroids = tagsCentroids;
		temp.normal = normal;
		outData.kdTreeData.push_back(temp);

#ifdef DISPLAY_POINT_PLANES
		std::cout << "\nPOINT " << i << " : " << std::endl;
		std::cout << "For query point " << queryPoint.tostring(constants::psd) << " with kRadius " << kRadius << std::endl;
		std::cout << "The neighborhood is the set " << neighbors.tostring(constants::psd) << std::endl;
		std::cout << "The centroid is " << centroid.tostring(constants::psd) << std::endl;
		std::cout << "And the normal is " << normal.tostring(constants::psd) << std::endl;
		std::cout << "With tag index " << tagsCentroids[i] << std::endl << std::endl;
#endif  //DISPLAY_POINT_PLANES
	}





	/*
	*	CONSISTENTLY ORIENT NORMALS
	*
	*		1. build kdtree of centroids
	*		2. build a Riemannian Graph of centroids
	*
	*			- two centroids are connected in the plane
	*			  if they are on each other's neighborhood
	*
	*			- if they are connected, the weight is
	*			  1 - | normal of u dot normal of v |
	*
	*			- otherwise it is INF
	*
	*		3. use Prim's Algorithm to generate the MST
	*		   of the Riemannian Graph
	*
	*		4. consistently orient normals along the MST
	*/

	std::cout << "Building Riemannian Graph of centroids..." << std::endl;

	alglib::kdtree kdtCentroids;
	globals::kdtCentroids = &kdtCentroids;
	kdtmanip::buildTaggedKDTree(kdtCentroids, centroids, tagsCentroids, nPoints);

	// keep tab of max normal z component
	alglib::ae_int_t mstRootIdx = 0;
	double mstRootZVal = abs(normals[0][2]);

	// build a graph where all neighboring centroids are connected
	double** graph = new double*[nPoints];

	for (alglib::ae_int_t u = 0; u < nPoints; u++)
	{
		graph[u] = new double[nPoints];

		// update max z componentto to root mst
		if (mstRootZVal < abs(normals[u][2]))
		{
			mstRootZVal = abs(normals[u][2]);
			mstRootIdx = u;
		}

		// take the u point for query
		alglib::real_1d_array queryCentroid = ops::makeArr(centroids[u], constants::dims);

		// query the kdtree for the neighbors
		alglib::ae_int_t k = alglib::kdtreequeryrnn(kdtCentroids, queryCentroid, kRadius);

		// store the tags that contain the point indices
		alglib::integer_1d_array uNeighTags;
		uNeighTags.setlength(k);
		alglib::kdtreequeryresultstags(kdtCentroids, uNeighTags);

		// store neighbors in a set to do log(n) lookups
		alglib::ae_int_t *intTags = uNeighTags.getcontent();
		std::set<alglib::ae_int_t> setTags(intTags, intTags + k);

		// now build the adjacency row for point u
		for (alglib::ae_int_t v = 0; v < nPoints; v++)
		{
			double weight = DBL_MAX;

			if (u != v)
			{
				std::set<int>::iterator it = setTags.find(v);
				// if v is in u's neighborhood, add an edge with inverse normal weight
				if (it != setTags.end())
					weight = 1 - abs(ops::dot(normals[u], normals[v], constants::dims));
			}

			graph[u][v] = weight;
		}
	}


//#define PRINT_RIEMANNNIAN_GRAPH
#ifdef PRINT_RIEMANNNIAN_GRAPH
	std::cout << "\nRiemannian graph of centroids with w(u,v) = 1-|n_u . n_v| :" << std::endl;
	graph::printGraph(graph, nPoints, nPoints);
#endif

	// create the graph mst successor array
	size_t *graphMst = new size_t[nPoints];
	graph::primMst(graph, nPoints, graphMst);

#ifdef PRINT_MST
	std::cout << "\nMinimun spanning tree centroids with w(u,v) = 1-|n_u . n_v| :" << std::endl;
	graph::printMst(graphMst, nPoints, graph, normals);
#endif

#ifdef SHOW_NORMALS
	std::cout << "\n\nNormals before propagation:" << std::endl;
	std::cout << normals.tostring(constants::psd) << std::endl;
#endif

	// then propagate
	std::cout << "Propagating normal orientations rooted at " << mstRootIdx << "..." << std::endl;

	// align with z+
	normals[mstRootIdx][0] = 0.0;
	normals[mstRootIdx][1] = 0.0;
	normals[mstRootIdx][2] = 1.0;

	for (size_t i = 0; i < nPoints; i++)
	{
		if (graphMst[i] == -1)
			mstRootIdx = i;
	}

	graph::propagateNormals(graphMst, nPoints, normals, mstRootIdx);    

#ifdef PRINT_MST
	std::cout << "\nMinimun spanning tree centroids with w(u,v) = 1-|n_u . n_v| :" << std::endl;
	graph::printMst(graphMst, nPoints, graph, normals);
#endif

	// gather data for rendering
	for (size_t i = 0; i < nPoints; i++) 
	{
		alglib::real_1d_array t;
		t.setcontent(constants::dims, normals[i]);
		outData.kdTreeData[i].normalOriented = t;
	}

#ifdef SHOW_NORMALS
	std::cout << "\n\nNormals after propagation:" << std::endl;
	std::cout << normals.tostring(constants::psd) << std::endl;
#endif
	
	outData.rGraph = graph;









	globals::cubesData = generateMcData(points, nPoints, kRadius);
	// maxes and mines for timmy to scale the thing
	outData.minX = globals::cubesData->mcMinX;
	outData.maxX = globals::cubesData->mcMaxX;
	outData.minY = globals::cubesData->mcMinY;
	outData.maxY = globals::cubesData->mcMaxY;
	outData.minZ = globals::cubesData->mcMinZ;
	outData.maxZ = globals::cubesData->mcMaxZ;


	runMarchingCubes();





					//x,y,z
	/*int cells[3] = { 2,2,2 };
	globals::cubesData = generateMcData(points, nPoints, kRadius);

	// maxes and mines for timmy to scale the thing
	outData.minX = globals::cubesData->mcMinX;
	outData.maxX = globals::cubesData->mcMaxX;
	outData.minY = globals::cubesData->mcMinY;
	outData.maxY = globals::cubesData->mcMaxY;
	outData.minZ = globals::cubesData->mcMinZ;
	outData.maxZ = globals::cubesData->mcMaxZ;

	std::cout << globals::cubesData->tostr();

	TRIANGLE *t;
	int nt = runMarchingCubes(t);


	std::cout << "number of triangles:" << nt << std::endl;
	std::cout << "distance called:" << globals::dn << std::endl;

	for (int i = 0; i < nt; i++)
	{
		std::cout << "T" << i << " " << t[i].tostr() << std::endl;
	}*/

	// clean up memory
	// delete[] graphMst;
	// graph::deleteAdjMatrix(graph, nPoints);


	



	return true;
}