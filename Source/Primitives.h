#pragma once
#include <glm/vec3.hpp>

namespace Primitives
{
	typedef struct TriangleFace
	{
		TriangleFace
		(
			const glm::vec3 & v1, 
			const glm::vec3 & v2, 
			const glm::vec3 & v3, 
			const glm::vec3 & n1, 
			const glm::vec3 & n2, 
			const glm::vec3 & n3
		)
			: v1(v1)
			, v2(v2)
			, v3(v3)
			, n1(n1)
			, n2(n2)
			, n3(n3)
		{

		}

		glm::vec3 v1;
		glm::vec3 v2;
		glm::vec3 v3;
		glm::vec3 n1;
		glm::vec3 n2;
		glm::vec3 n3;
	} TriangleFace;

	typedef struct SegmentLine
	{
		SegmentLine
		(
			const glm::vec3 & v1, 
			const glm::vec3 & v2
		)
			: v1(v1)
			, v2(v2)
		{

		}

		glm::vec3 v1;
		glm::vec3 v2;
	} SegmentLine;

	typedef struct Point
	{
		Point
		(
			const glm::vec3 & v1
		)
			: v1(v1)
		{

		}

		glm::vec3 v1;
	} Point;
}