#include "david_graph.h"

size_t graph::minKey(double *key, bool *mstSet, size_t n)
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
void graph::printMst(size_t *parent, size_t n, double **graph, alglib::real_2d_array& normals)
{
	alglib::ae_int_t dims = normals.cols();
	for (int i = 1; i < n; i++)
	{
		std::cout << parent[i] << "-" << i << " : " << graph[i][parent[i]];
		std::cout << "  [" << normals[i][0] << "," << normals[i][1] << "," << normals[i][2] << "]";
		std::cout << " - [" << normals[parent[i]][0] << "," << normals[parent[i]][1] << "," << normals[parent[i]][2] << "]";
		std::cout << " - dot: " << ops::dot(normals[i], normals[parent[i]], dims) << std::endl;
	}
}


size_t* graph::primMst(double **graph, const size_t n, size_t *parent)
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


void graph::propagateNormals(size_t *graphMst, size_t nPoints, alglib::real_2d_array& normals, size_t root)
{
	// dimensions
	alglib::ae_int_t dims = normals.cols();

	for (size_t u = 0; u < nPoints; u++) // for every vertex u
		if (graphMst[u] == root) // if u vertex is a child of root
		{
			// dot product of normals parent * child < 0, flip normal
			if (ops::dot(normals[root], normals[u], dims) < 0)
				ops::flip(normals[u], dims);

			// then propagate along u
			propagateNormals(graphMst, nPoints, normals, u);
		}
}

void graph::printMatrix(double **matrix, const size_t rows, const size_t cols)
{
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
			std::cout << std::left << std::setw(16) << matrix[i][j];

		std::cout << std::endl;
	}
}

void graph::printGraph(double **matrix, const size_t rows, const size_t cols)
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

void graph::deleteAdjMatrix(double ** arr, const size_t n)
{
	for (size_t i = 0; i < n; i++)
		delete[] arr[i];

	delete[] arr;
}