#pragma once

#include <string>

// tiny object loader
#define TINYOBJLOADER_IMPLEMENTATIO
#include "../tinyobj/tiny_obj_loader.h";

// alglib types
#include "../alglib/alglibmisc.h";

// other stuff
#include "david_constants.h";

namespace cloudfile
{
	// getting, loading, and saving cloud

	/*
		loadCloud

			Wraps the process of loading the cloud point
			by reading vertices from the OBJ file using
			tinyobj loader.

			This can easily be extended to other formats.

			Consider CSV, PLY, etc.
	*/

	bool loadCloud(tinyobj::attrib_t& attrib, const std::string& filename);


	/*
		getCloudPointFilename

			takes in the OBJ model filename
	*/

	std::string getCloudPointFilename(std::string basePath = constants::cloudPointsBasePath);

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

	alglib::real_2d_array& adaptDataPoints(const std::vector<tinyobj::real_t>& vertices, const size_t numberOfVertices, alglib::real_2d_array& points);
}