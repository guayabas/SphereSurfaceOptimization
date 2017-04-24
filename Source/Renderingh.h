#pragma once
#include "Primitives.h"
#include <vector>

namespace Rendering
{
	using namespace Primitives;

	void glDrawPointCloud(const std::vector<glm::vec3> &, const glm::vec3 & = glm::vec3(0.0f), bool = false);
	void glDrawConvexHull(const std::vector<glm::vec3> &);
	void glDrawCircle(unsigned = 0);

	void glDrawSegmentLine(const SegmentLine &, const float & = 1.0f);
	void glDrawTriangle(const TriangleFace &, const float & = 1.0f);
	void glDrawPoint(const Point &, const float & = 1.0f);

	void glDisableLight();
	void glEnableLight();
}