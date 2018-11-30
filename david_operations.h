#pragma once

#include <algorithm>

// alglib nearest neighbor subpackage for kdtree
#include "../alglib/alglibmisc.h";
#include "../alglib/dataanalysis.h";
#include "../alglib/ap.h"


namespace ops 
{
	template <typename T> inline T add(const T& a, const T& b) { return a + b; }
	template <typename T> inline T subtract(const T& a, const T& b) { return a - b; }
	template <typename T> inline T multiply(const T& a, const T& b) { return a * b; }

	inline double sum(const alglib::real_1d_array& a);
	double dot(const alglib::real_1d_array& a, const alglib::real_1d_array& b);
	double dot(const double const * a, const double const * b, const size_t dims);
	alglib::real_1d_array operate(const alglib::real_1d_array& a, const alglib::real_1d_array& b, double operation(const double&, const double&));
	alglib::real_1d_array& zero(alglib::real_1d_array& a);
	alglib::real_1d_array mean(const alglib::real_2d_array& points);
	alglib::real_1d_array makeArr(const alglib::ae_int_t n, bool zero = false);
	alglib::real_1d_array makeArr(const double * const content, const alglib::ae_int_t n);
	double* flip(double * const v, const size_t dims = 3);
	alglib::real_1d_array& flip(alglib::real_1d_array& v);

	/*
		calculateCentroid

			Estimates the center to the
			best fitting plane of a set
			of points in R(constants::dims) space.
			This centroid is the mean vector
			of the set of points.
	*/

	alglib::real_1d_array getCentroid(const alglib::real_2d_array & points);


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

	alglib::real_1d_array getNormal(const alglib::real_2d_array & points);
	double * copyArr(double * dest, alglib::real_1d_array & source);
}

alglib::real_1d_array operator+(const alglib::real_1d_array& a, const alglib::real_1d_array& b);
alglib::real_1d_array operator*(const alglib::real_1d_array& a, const alglib::real_1d_array& b);
alglib::real_1d_array operator-(const alglib::real_1d_array& a, const alglib::real_1d_array& b);
