#pragma once

#pragma once

//                 v7_______e6_____________v6
//                  /|                    /|
//                 / |                   / |
//              e7/  |                e5/  |
//               /___|______e4_________/   |
//            v4|    |                 |v5 |e10
//              |    |                 |   |
//              |    |e11              |e9 |
//            e8|    |                 |   |
//              |    |_________________|___|
//              |   / v3      e2       |   /v2
//              |  /                   |  /
//              | /e3                  | /e1
//              |/_____________________|/
//              v0         e0          v1


#include "libraries/meshgen/MeshReconstruction.h"
#include "libraries/meshgen/IO.h"

// alglib nearest neighbor subpackage for kdtree
#include "../alglib/alglibmisc.h";
#include "../alglib/dataanalysis.h";
#include "../alglib/ap.h"

#include "david_operations.h"
#include <limits>
#include <iostream>


struct mcData {
	float mcMinX;
	float mcMaxX;
	float mcMinY;
	float mcMaxY;
	float mcMinZ;
	float mcMaxZ;
	float minValue;

	std::string tostr()
	{
		std::stringstream s;

		s << "MCDATA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
		s << "mcMinX   :" << mcMinX << std::endl;
		s << "mcMaxX   :" << mcMaxX << std::endl;
		s << "mcMinY   :" << mcMinY << std::endl;
		s << "mcMaxY   :" << mcMaxY << std::endl;
		s << "mcMinZ   :" << mcMinZ << std::endl;
		s << "mcMaxZ   :" << mcMaxZ << std::endl;
		s << "minValue :" << minValue << std::endl;
		s << "MCDATA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

		return s.str();
	}
};


namespace globals
{
	alglib::kdtree* kdtCentroids;
	alglib::real_2d_array* normals;
	alglib::real_2d_array* centroids;
	mcData* cubesData;
	double* radius;
	size_t dn = 0;
};


alglib::real_1d_array Vec3_alglib(MeshReconstruction::Vec3 const& p)
{
	double v[3] = {
		p.x,
		p.y,
		p.z
	};

	return ops::makeArr(v, 3);
}

MeshReconstruction::Vec3 alglib_Vec3(const alglib::real_1d_array& arr)
{
	MeshReconstruction::Vec3 v;
	v.x = arr[0];
	v.y = arr[1];
	v.z = arr[2];
	return v;
}


bool getClosest(size_t& idx, alglib::real_1d_array& qP)
{
	double& r = *globals::radius;
	alglib::kdtree& kdt = *globals::kdtCentroids;
	alglib::real_2d_array& normals = *globals::normals;

	// query the kdtree for the neighbors
	alglib::ae_int_t k = alglib::kdtreequeryrnn(kdt, qP, r);

	// store the tags that contain the point indices
	alglib::integer_1d_array uNeighTags;
	uNeighTags.setlength(k);
	alglib::kdtreequeryresultstags(kdt, uNeighTags);

	if (k > 0)
	{
		idx = uNeighTags[0];
		return true;
	}

	return false;
}

float signedDistance(double* n, double* o, const double* p)
{
	double diff[3] = {
		p[0] - o[0],
		p[1] - o[1],
		p[2] - o[2]
	};

	double d = ops::dot(diff, n, 3);

	return d;
}

float formula(MeshReconstruction::Vec3 const& p)
{
	alglib::real_1d_array qP = Vec3_alglib(p);
	size_t planeIdx;
	double d;

	double& r = *globals::radius;
	alglib::real_2d_array& o = *globals::centroids;
	alglib::real_2d_array& n = *globals::normals;

	if (getClosest(planeIdx, qP))
	{
		d = signedDistance(n[planeIdx], o[planeIdx], qP.getcontent());
		if (d >= 0.f && d < 0.0005) d += 0.0005;
		if (d < 0.f && d > -0.0005) d -= 0.0005;

		/*std::cout << "distance_a: " << d << " , plane: " << planeIdx << " at: [";
		std::cout << o[planeIdx][0] << ", ";
		std::cout << o[planeIdx][1] << ", ";
		std::cout << o[planeIdx][2] << "] ";
		std::cout << " with normal: [";
		std::cout << n[planeIdx][0] << ", ";
		std::cout << n[planeIdx][1] << ", ";
		std::cout << n[planeIdx][2] << "] ";
		std::cout << " queryPoint: " << qP.tostring(5) << std::endl;*/
	}
	else
	{
		d = r + 1.f;
		/*std::cout << "distance_b: " << d << std::endl;*/
	}

	globals::dn++;
	return d;
}



mcData* generateMcData(alglib::real_2d_array& points, size_t n, float minVal)
{
	mcData *data = new mcData;

	data->mcMinX = std::numeric_limits<float>::infinity();
	data->mcMaxX = -std::numeric_limits<float>::infinity();
	data->mcMinY = std::numeric_limits<float>::infinity();
	data->mcMaxY = -std::numeric_limits<float>::infinity();
	data->mcMinZ = std::numeric_limits<float>::infinity();
	data->mcMaxZ = -std::numeric_limits<float>::infinity();

	for (size_t i = 0; i < n; i++)
	{
		if (points[i][0] <= data->mcMinX) data->mcMinX = points[i][0];
		if (points[i][0] >= data->mcMaxX) data->mcMaxX = points[i][0];
		if (points[i][1] <= data->mcMinY) data->mcMinY = points[i][1];
		if (points[i][1] >= data->mcMaxY) data->mcMaxY = points[i][1];
		if (points[i][2] <= data->mcMinZ) data->mcMinZ = points[i][2];
		if (points[i][2] >= data->mcMaxZ) data->mcMaxZ = points[i][2];
	}

	data->minValue = minVal;

	return data;
}


void runMarchingCubes()
{
	/*auto sphereSdf = [](MeshReconstruction::Vec3 const& pos)
	{
		auto const Radius = 1.0;
		return pos.Norm() - Radius;
	};*/

	MeshReconstruction::Rect3 domain;
	domain.min = { -2.1, -2.1, -2.1 };
	domain.size = { 5.5, 5.5, 5.5 };

	MeshReconstruction::Vec3 cubeSize{ 0.05, 0.05, 0.05 };

	auto mesh = MeshReconstruction::MarchCube(formula, domain, cubeSize);
	WriteObjFile(mesh, "test.obj");
}