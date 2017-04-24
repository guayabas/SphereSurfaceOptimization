#pragma once
#include <nlopt/api/nlopt.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace PointDistribution
{
	typedef nlopt_algorithm alg;

	std::pair<unsigned, float> run_2d
	(
		std::vector<glm::vec2> &,
		std::vector<glm::vec2> &,
		alg = NLOPT_LN_COBYLA,
		const unsigned = 1000,
		const unsigned = 2,
		const unsigned = 0,
		bool = false,
		bool = true,
		bool = true
	);

	std::pair<unsigned, float> run_3d
	(
		std::vector<glm::vec3> &,
		std::vector<glm::vec3> &,
		alg = NLOPT_LN_COBYLA,
		const unsigned = 1000,
		const unsigned = 2, 
		const unsigned = 0,
		bool = false,
		bool = true, 
		bool = true,
		bool = true,
		bool = true
	);
}