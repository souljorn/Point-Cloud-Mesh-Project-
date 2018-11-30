#include "david_kdtmanip.h"

alglib::kdtree& kdtmanip::buildKDTree(alglib::kdtree& kdt, const alglib::real_2d_array& points, const alglib::ae_int_t n)
{
	alglib::ae_int_t xdims = constants::dims, ydims = 0, normType = constants::kdtTreeNormType;
	alglib::kdtreebuild(points, n, xdims, ydims, normType, kdt);
	return kdt;
}

alglib::kdtree& kdtmanip::buildTaggedKDTree(alglib::kdtree& kdt, const alglib::real_2d_array& points, const alglib::integer_1d_array& tags, const alglib::ae_int_t n)
{
	alglib::ae_int_t xdims = constants::dims, ydims = 0, normType = constants::kdtTreeNormType;
	alglib::kdtreebuildtagged(points, tags, n, xdims, ydims, normType, kdt);
	return kdt;
}

alglib::ae_int_t kdtmanip::getKNeighbors(alglib::kdtree& kdt, alglib::real_2d_array& result, const alglib::real_1d_array& queryPoint, double radius)
{
	// query tree for a given point around a radius
	alglib::ae_int_t k = alglib::kdtreequeryrnn(kdt, queryPoint, radius);

	// the queried results are in an internal buffer of the tree
	result.setlength(k, constants::dims);
	alglib::kdtreequeryresultsx(kdt, result);

	// return the size of neighbor set
	return k;
}

alglib::ae_int_t kdtmanip::getKNeighborsTagged(alglib::kdtree& kdt, alglib::real_2d_array& result, alglib::integer_1d_array& tags, const alglib::real_1d_array& queryPoint, double radius)
{
	// query tree for a given point around a radius
	alglib::ae_int_t k = alglib::kdtreequeryrnn(kdt, queryPoint, radius);

	// the queried results are in an internal buffer of the tree
	result.setlength(k, constants::dims);
	tags.setlength(k);

	alglib::kdtreequeryresultsx(kdt, result);
	alglib::kdtreequeryresultstags(kdt, tags);

	// return the size of neighbor set
	return k;
}