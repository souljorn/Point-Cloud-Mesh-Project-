#pragma once

#include <string>

namespace constants
{
	const std::string cloudPointsBasePath = "PointClouds\\";
	const unsigned int kdtTreeNormType = 2; // 2-norm (Euclidean-norm)
	const unsigned int dims = 3; // dimensions
	const unsigned int psd = 6; // precision display, used on displaying alglib f-values
}