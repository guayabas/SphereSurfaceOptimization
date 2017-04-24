#pragma once
#include "Primitives.h"
#include <vector>

class RecursiveSphere
{
public:
	RecursiveSphere(); ~RecursiveSphere();
	void draw(float = 1.0f, bool = false, float = 1.0f) const;

	Primitives::TriangleFace getFace(unsigned) const;

	void recompute(unsigned);

protected:
	void subdivideTriangle(const glm::vec3 &, const glm::vec3 &, const glm::vec3 &, unsigned);
	void clearFaces();

private:
	std::vector<Primitives::TriangleFace> m_faces;
	unsigned m_level;
};