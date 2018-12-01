#pragma once

//#define DISPLAY_GRAPH

#include "../alglib/alglibmisc.h";

#include <float.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "david_operations.h"

namespace graph
{
	size_t minKey(double * key, bool * mstSet, size_t n);
	void printMst(size_t * parent, size_t n, double ** graph, alglib::real_2d_array & normals);
	size_t * primMst(double ** graph, const size_t n, size_t * parent);
	void propagateNormals(size_t * graphMst, size_t nPoints, alglib::real_2d_array & normals, size_t root);
	void printMatrix(double ** matrix, const size_t rows, const size_t cols);
	void printGraph(double ** matrix, const size_t rows, const size_t cols);
	void deleteAdjMatrix(double ** arr, const size_t n);
}