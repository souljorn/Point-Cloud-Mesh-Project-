#include "david_cloudfile.h"

bool cloudfile::loadCloud(tinyobj::attrib_t& attrib, const std::string& filename)
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


std::string cloudfile::getCloudPointFilename(std::string basePath)
{
	std::string filename = "red_pepper_down.obj";
	/*std::cout << "Please input cloud point filename: ";
	std::cin >> filename;*/


	return basePath + filename;
}


alglib::real_2d_array& cloudfile::adaptDataPoints(const std::vector<tinyobj::real_t>& vertices, const size_t numberOfVertices, alglib::real_2d_array& points)
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