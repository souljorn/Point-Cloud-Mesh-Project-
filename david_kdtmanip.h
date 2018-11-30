#pragma once


#include "david_constants.h"
#include "../alglib/alglibmisc.h";


namespace kdtmanip
{
	/*
		buildKDTree

			Build a constants::dims dimensional kdtree
			of the cloud point using norm2
			(euclidean distance).
	*/

	alglib::kdtree& buildKDTree(alglib::kdtree& kdt, const alglib::real_2d_array& points, const alglib::ae_int_t n);

	/*
		buildTaggedKDTree

			Build a constants::dims dimensional kdtree
			of the cloud point using norm2
			(euclidean distance). Includes a tag.
			Used commonly to store point index.
	*/

	alglib::kdtree& buildTaggedKDTree(alglib::kdtree& kdt, const alglib::real_2d_array& points, const alglib::integer_1d_array& tags, const alglib::ae_int_t n);

	/*
		getKNeighbors

			get k neighbors on a given radius of the query point
			(returns self if only point in the k-neighborhood)
	*/

	alglib::ae_int_t getKNeighbors(alglib::kdtree& kdt, alglib::real_2d_array& result, const alglib::real_1d_array& queryPoint, double radius);

	/*
		getKNeighborsTagged

			get k neighbors on a given radius of the query point
			(returns self if only point in the k-neighborhood)
			includes tags
	*/

	alglib::ae_int_t getKNeighborsTagged(alglib::kdtree& kdt, alglib::real_2d_array& result, alglib::integer_1d_array& tags, const alglib::real_1d_array& queryPoint, double radius);
}