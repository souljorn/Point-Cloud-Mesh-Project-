#pragma once


// David's toolbox
#include <fstream>
#include <iomanip>
#include <windows.h>
#include <set>
#include <stdlib.h>
#include "cubesmagic.h"

// adjust verbosity for debugging
//#define VERY_VERBOSE
//#define DISPLAY_POINT_PLANES 0
//#define OUTPUT_TO_FILE 0

// tiny object loader
#define TINYOBJLOADER_IMPLEMENTATIO
#include "../tinyobj/tiny_obj_loader.h";

// alglib nearest neighbor subpackage for kdtree
#include "../alglib/alglibmisc.h";
#include "../alglib/dataanalysis.h";
#include "../alglib/ap.h"



namespace constants
{
	const std::string cloudPointsBasePath = "PointClouds\\";
	const unsigned int kdtTreeNormType = 2; // 2-norm (Euclidean-norm)
	const unsigned int dims = 3; // dimensions
	const unsigned int psd = 6; // precision display, used on displaying alglib f-values
}


/*
	loadCloud

		Wraps the process of loading the cloud point
		by reading vertices from the OBJ file using
		tinyobj loader.

		This can easily be extended to other formats.

		Consider CSV, PLY, etc.
*/

bool loadCloud(tinyobj::attrib_t& attrib, const std::string& filename)
{
	// dummy vectors... we won't use them, we only need vertex info
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	// error string buffer
	std::string err, warn;

	// load the obj file
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), NULL, false, true);

	// display errors and warnings
	if (!err.empty()) {
		std::cerr << err << std::endl;
	}
	if (!warn.empty()) {
		std::cerr << warn << std::endl;
	}

	return ret;
}


/*
	getCloudPointFilename

		takes in the OBJ model filename
*/

std::string getCloudPointFilename(std::string basePath = constants::cloudPointsBasePath)
{
	std::string filename = "red_pepper_down.obj";
	/*std::cout << "Please input cloud point filename: ";
	std::cin >> filename;*/


	return basePath + filename;
}

/*
	adaptDataPoints

		The input OBJ file is stored with
		float precisition but ALGLIB uses
		double for its R1 representations.

		Casting the values is ok but casting
		pointers generates missalignment.

		We could also set TINYOBJLOADER_USE_DOUBLE
		to change the real_t type of tinyobj,
		however, this introduces problems
		when reading the files.
*/

alglib::real_2d_array& adaptDataPoints(const std::vector<tinyobj::real_t>& vertices, const size_t numberOfVertices, alglib::real_2d_array& points)
{
	points.setlength(numberOfVertices, constants::dims);

	for (size_t vertex = 0; vertex < numberOfVertices; vertex++)
	{
		// write d consequent values
		size_t marker = vertex * constants::dims;
		for (size_t d = 0; d < constants::dims; d++)
			points[vertex][d] = vertices[marker + d];
	}

	return points;
}

/*
	buildKDTree

		Build a constants::dims dimensional kdtree
		of the cloud point using norm2
		(euclidean distance).
*/

alglib::kdtree& buildKDTree(alglib::kdtree& kdt, const alglib::real_2d_array& points, const alglib::ae_int_t n)
{
	alglib::ae_int_t xdims = constants::dims, ydims = 0, normType = constants::kdtTreeNormType;
	alglib::kdtreebuild(points, n, xdims, ydims, normType, kdt);
	return kdt;
}

alglib::kdtree& buildTaggedKDTree(alglib::kdtree& kdt, const alglib::real_2d_array& points, const alglib::integer_1d_array& tags, const alglib::ae_int_t n)
{
	alglib::ae_int_t xdims = constants::dims, ydims = 0, normType = constants::kdtTreeNormType;
	alglib::kdtreebuildtagged(points, tags, n, xdims, ydims, normType, kdt);
	return kdt;
}

/*
	getKNeighbors

		get k neighbors on a given radius of the query point
		(returns self if only point in the k-neighborhood)
*/

alglib::ae_int_t getKNeighbors(alglib::kdtree& kdt, alglib::real_2d_array& result, const alglib::real_1d_array& queryPoint, double radius)
{
	// query tree for a given point around a radius
	alglib::ae_int_t k = alglib::kdtreequeryrnn(kdt, queryPoint, radius);

	// the queried results are in an internal buffer of the tree
	result.setlength(k, constants::dims);
	alglib::kdtreequeryresultsx(kdt, result);

	// return the size of neighbor set
	return k;
}

/*
	getKNeighborsTagged

		get k neighbors on a given radius of the query point
		(returns self if only point in the k-neighborhood)
		includes tags
*/

alglib::ae_int_t getKNeighborsTagged(alglib::kdtree& kdt, alglib::real_2d_array& result, alglib::integer_1d_array& tags, const alglib::real_1d_array& queryPoint, double radius)
{
	// query tree for a given point around a radius
	alglib::ae_int_t k = alglib::kdtreequeryrnn(kdt, queryPoint, radius);

	// the queried results are in an internal buffer of the tree
	result.setlength(k, constants::dims);
	tags.setlength(k);

	alglib::kdtreequeryresultsx(kdt, result);
	alglib::kdtreequeryresultstags(kdt, tags);

	//std::cout << tags.tostring() << std::endl;

	// return the size of neighbor set
	return k;
}

/*
	calculateCentroid

		Estimates the center to the
		best fitting plane of a set
		of points in R(constants::dims) space.
		This centroid is the mean vector
		of the set of points.
*/

alglib::real_1d_array calculateCentroid(const alglib::real_2d_array& points, alglib::ae_int_t k)
{
	// the centroid is the mean vector of the neighbors
	alglib::real_1d_array centroid;
	centroid.setlength(constants::dims);

	// initialize
	for (size_t d = 0; d < constants::dims; d++)
		centroid[d] = 0;

	// sum mean vector
	for (alglib::ae_int_t i = 0; i < k; i++)
		for (size_t d = 0; d < constants::dims; d++)
			centroid[d] += points[i][d];

	// mean
	for (size_t d = 0; d < constants::dims; d++)
		centroid[d] /= k;

	return centroid;
}

/*
	calculateNormal

		Estimates the normal to the
		best fitting plane of a set
		of points in R(constants::dims) space.
		This normal is the eigenvector
		corresponding to the least
		eigenvalue of the covariance
		matrix. Basically, PCA.
*/

alglib::real_1d_array calculateNormal(const alglib::real_2d_array& points, alglib::ae_int_t k)
{
	// will store normal estimation to return
	alglib::real_1d_array normal;

	// pcaInfo returns 1 if valid
	alglib::ae_int_t pcaInfo;

	// pcaS2 -> eigenvalues in descending order
	// pcaV  -> eigenvectors in corresponding order
	alglib::real_1d_array pcaS2;
	alglib::real_2d_array pcaV;

	// perform full pca on the points
	alglib::pcabuildbasis(points, k, constants::dims, pcaInfo, pcaS2, pcaV);

	// set normal to last eigenvector
	normal.setcontent(constants::dims, pcaV[constants::dims - 1]);

	return normal;
}



size_t minKey(double *key, bool *mstSet, size_t n)
{
	// initialize min value 
	double min = DBL_MAX;
	size_t min_index;

	for (size_t v = 0; v < n; v++)
		if (mstSet[v] == false && key[v] <= min)
			min = key[v], min_index = v;

	return min_index;
}

// A utility function to print the  
// constructed MST stored in parent[] 
void printMST(size_t *parent, size_t n, double **graph, alglib::real_2d_array& normals)
{
	for (int i = 1; i < n; i++)
	{

#ifdef DISPLAY_GRAPH
		std::cout << parent[i] << "-" << i << " : " << graph[i][parent[i]];
		std::cout << "  [" << normals[i][0] << "," << normals[i][1] << "," << normals[i][2] << "]";
		std::cout << " - [" << normals[parent[i]][0] << "," << normals[parent[i]][1] << "," << normals[parent[i]][2] << "]";
#endif
		
		double dot = 0;
		dot += normals[i][0] * normals[parent[i]][0];
		dot += normals[i][1] * normals[parent[i]][1];
		dot += normals[i][2] * normals[parent[i]][2];

#ifdef DISPLAY_GRAPH
		std::cout << " - dot: " << dot << std::endl;
#endif
	}
}


size_t* primMst(double **graph, const size_t n, size_t *parent)
{
	// book keeping 
	double *key = new double[n];	// key values used to pick minimum weight edge in cut
	bool *mstSet = new bool[n];		// pending mst vertices

	// initialize all keys to inf 
	for (size_t i = 0; i < n; i++)
	{
		key[i] = DBL_MAX;
		mstSet[i] = false;
	}

	key[0] = 0.0;
	parent[0] = -1;

	for (size_t count = 0; count < n - 1; count++)
	{
		size_t u = minKey(key, mstSet, n); // pick min of non used
		mstSet[u] = true; // mark it in the MST

		// update keys for best current edges 
		for (int v = 0; v < n; v++)
			if (mstSet[v] == false && graph[u][v] <= key[v])
				parent[v] = u, key[v] = graph[u][v];
	}

	delete[] key;
	delete[] mstSet;

	return parent;
}


void propagateNormals(size_t *graphMst, size_t nPoints, alglib::real_2d_array& normals, size_t root)
{
	for (size_t u = 0; u < nPoints; u++)
	{
		if (graphMst[u] == root) // if u node is a child of root
		{
			// dot product of normals parent * child
			double dot = 0;
			dot += normals[root][0] * normals[u][0];
			dot += normals[root][1] * normals[u][1];
			dot += normals[root][2] * normals[u][2];

#ifdef DISPLAY_GRAPH
			std::cout << "(" << u << "," << root << ") ";
			std::cout << "[" << normals[root][0] << "," << normals[root][1] << "," << normals[root][2] << "]";
			std::cout << " - [" << normals[u][0] << "," << normals[u][1] << "," << normals[u][2] << "]";
			std::cout << " - dot: " << dot << std::endl;
#endif

			// flip if neccesary
			if (dot < 0)
			{

#ifdef DISPLAY_GRAPH
				std::cout << "!!!!!!!!!!!!! Flipped!";
				std::cout << " - [" << normals[u][0] << "," << normals[u][1] << "," << normals[u][2] << "]";
#endif
				normals[u][0] *= -1;
				normals[u][1] *= -1;
				normals[u][2] *= -1;
#ifdef DISPLAY_GRAPH
				std::cout << " ->>> [" << normals[u][0] << "," << normals[u][1] << "," << normals[u][2] << "]";
#endif
			}

			// then propagate along u
			propagateNormals(graphMst, nPoints, normals, u);
		}
	}
}

void printMatrix(double **matrix, const size_t rows, const size_t cols)
{
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
			std::cout << std::left << std::setw(16) << matrix[i][j];

		std::cout << std::endl;
	}
}

void printGraph(double **matrix, const size_t rows, const size_t cols)
{
	std::cout << std::left << std::setw(8) << "vertex |";
	for (size_t j = 0; j < cols; j++)
		std::cout << std::left << std::setw(16) << j;
	std::cout << std::endl;

	std::cout << std::left << std::setw(8) << "-------+";
	for (size_t j = 0; j < cols; j++)
		std::cout << std::left << std::setw(16) << "----------------";
	std::cout << std::endl;

	for (size_t i = 0; i < rows; i++)
	{
		std::cout << std::left << std::setw(7) << i << "|";
		for (size_t j = 0; j < cols; j++)
			std::cout << std::left << std::setw(16) << matrix[i][j];

		std::cout << std::endl;
	}
}

template <typename T>
void deleteDoubleArray(T **arr, const size_t n)
{
	for (size_t i = 0; i < n; i++)
		delete[] arr[i];

	delete[] arr;
}

double signedDistance(alglib::real_1d_array& point, alglib::real_1d_array& normal, alglib::real_1d_array& centroid, double radius)
{
	return 0.0;
}

struct KDTreeData{
	

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
	size_t nPoints;

	//Points
	alglib::real_2d_array points;

	//Radius
	double kRadius;

	//KD Tree Data
	std::vector<KDTreeData> kdTreeData;

};


OutData davidsmagic()
{

#ifdef OUTPUT_TO_FILE
	std::ofstream out("./out.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
#endif

	/*
	*	LOAD THE OBJ FILE
	*/

	std::string filename = getCloudPointFilename();

	std::cout << "Loading " << filename << " wavefront file..." << std::endl;

	tinyobj::attrib_t pcloud;
	bool loaded = loadCloud(pcloud, filename);

	if (!loaded) {
		std::cerr << "ERROR: The OBJ file could not be loaded!" << std::endl;
		
	}






	/*
	*	CAST DATA POINTS
	*	TO DOUBLE FOR ALGLIB
	*
	*		cast data points to double and init real_2d_array for alglib kdtree
	*
	*/

	size_t nPoints = pcloud.vertices.size()/ constants::dims;

	std::cout << "Casting and adapting format on " << nPoints << " points..." << std::endl;

	alglib::real_2d_array points;
	adaptDataPoints(pcloud.vertices, nPoints, points);

	//Struct to export data points
	OutData outData;
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

	double kRadius = 10.0f;	// this should be a function of 
							// the density and noise of the point cloud

	outData.kRadius = kRadius;

	std::cout << "Building an " << nPoints << " points kdtree using norm2 (euclidean)..." << std::endl;

	// build kdtree
	alglib::kdtree kdt;
	buildKDTree(kdt, points, nPoints);

	// estimate planes
	std::cout << "Estimating planes under a neighborhood radius of " << kRadius << "..." << std::endl;

	// indexed normals
	alglib::real_2d_array normals;
	normals.setlength(nPoints, constants::dims);

	// indexed centroids
	alglib::real_2d_array centroids;
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
		alglib::real_1d_array queryPoint;
		queryPoint.setcontent(constants::dims, points[i]);

		

		// get the k neighbors of the point for a given radius
		alglib::real_2d_array neighbors;
		alglib::ae_int_t k;
		k = getKNeighbors(kdt, neighbors, queryPoint, kRadius);

		// calculate centroid
		alglib::real_1d_array centroid = calculateCentroid(neighbors, k);
		for (size_t d = 0; d < constants::dims; d++)
			centroids[i][d] = centroid.getcontent()[d];

		// keep centroids tag indexes to retrieve normals later
		tagsCentroids[i] = i;

		// calculate normal
		alglib::real_1d_array normal = calculateNormal(neighbors, k);
		for (size_t d = 0; d < constants::dims; d++)
			normals[i][d] = normal.getcontent()[d];


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
	buildTaggedKDTree(kdtCentroids, centroids, tagsCentroids, nPoints);

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
		alglib::real_1d_array queryCentroid;
		queryCentroid.setcontent(constants::dims, centroids[u]);

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
				{
					weight = 1;
					weight -= abs(normals[u][0] * normals[v][0]);
					weight -= abs(normals[u][1] * normals[v][1]);
					weight -= abs(normals[u][2] * normals[v][2]);
					weight = abs(weight);
				}
			}

			graph[u][v] = weight;
		}
	}


//#define PRINT_RIEMANNNIAN_GRAPH
#ifdef PRINT_RIEMANNNIAN_GRAPH
	std::cout << "\nRiemannian graph of centroids with w(u,v) = 1-|n_u . n_v| :" << std::endl;
	printGraph(graph, nPoints, nPoints);
#endif

	// create the graph mst successor array
	size_t *graphMst = new size_t[nPoints];
	primMst(graph, nPoints, graphMst);

//#define PRINT_MST
#ifdef PRINT_MST
	std::cout << "\nMinimun spanning tree centroids with w(u,v) = 1-|n_u . n_v| :" << std::endl;
	printMST(graphMst, nPoints, graph);
#endif

//#define SHOW_NORMALS
#ifdef SHOW_NORMALS
	std::cout << "\n\nNormals before propagation:" << std::endl;
	std::cout << normals.tostring(constants::psd) << std::endl;
#endif



	// then propagate
	printMST(graphMst, nPoints, graph, normals);
	std::cout << "########################################################" << std::endl;

	std::cout << "Propagating normal orientations rooted at " << mstRootIdx << "..." << std::endl;

	// align with z+
	normals[mstRootIdx][0] = 0.0;
	normals[mstRootIdx][1] = 0.0;
	normals[mstRootIdx][2] = 1.0;

	std::cout << "before:" << mstRootIdx << std::endl;
	for (size_t i = 0; i < nPoints; i++)
	{
		if (graphMst[i] == -1)
			mstRootIdx = i;
	}
	std::cout << "after:" << mstRootIdx << std::endl;
	std::cout << "roots predeccesor:" << graphMst[mstRootIdx] << std::endl;

	propagateNormals(graphMst, nPoints, normals, mstRootIdx);    

	printMST(graphMst, nPoints, graph, normals);
	std::cout << "########################################################" << std::endl;

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
#endif // VERY_VERBOSE





	// clean up memory
	delete[] graphMst;
	deleteDoubleArray(graph, nPoints);

	return outData;
}



/* test data */
/*nPoints = 6;
double inf = DBL_MAX;
double **graph = new double*[nPoints];
graph[0] = new double[nPoints] { inf, 1, inf, 3, inf, inf };
graph[1] = new double[nPoints] { 1, inf, 6, 5, 1, inf     };
graph[2] = new double[nPoints] { inf, 6, inf, inf, 5, 2   };
graph[3] = new double[nPoints] { 3, 5, inf, inf, 1, inf   };
graph[4] = new double[nPoints] { inf, 1, 5, 1, inf, 4     };
graph[5] = new double[nPoints] { inf, inf, 2, inf, 4, inf };*/

