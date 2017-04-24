#include <glm/geometric.hpp>
#include "RecursiveSphere.h"
#include "Renderingh.h"
#include <cmath>

RecursiveSphere::RecursiveSphere()
	: m_level(-1)
{

}

RecursiveSphere::~RecursiveSphere()
{
	clearFaces();
}

void RecursiveSphere::clearFaces()
{
	m_faces.clear();
}

void RecursiveSphere::recompute(unsigned level)
{
	if (level != m_level)
	{
		m_level = level;

		clearFaces();
		const unsigned numberFaces = unsigned(3 * std::pow(4, m_level));
		m_faces.reserve(numberFaces);

		glm::vec3 v1(+1.0f, +0.0f, +0.0f);
		glm::vec3 v2(+0.0f, +1.0f, +0.0f);
		glm::vec3 v3(+0.0f, +0.0f, +1.0f);
		glm::vec3 v4(-1.0f, +0.0f, +0.0f);
		glm::vec3 v5(+0.0f, -1.0f, +0.0f);
		glm::vec3 v6(+0.0f, +0.0f, -1.0f);

		subdivideTriangle(v1, v2, v3, m_level);
		subdivideTriangle(v2, v4, v3, m_level);
		subdivideTriangle(v4, v5, v3, m_level);
		subdivideTriangle(v5, v1, v3, m_level);
		subdivideTriangle(v2, v1, v6, m_level);
		subdivideTriangle(v4, v2, v6, m_level);
		subdivideTriangle(v5, v4, v6, m_level);
		subdivideTriangle(v1, v5, v6, m_level);
	}
}

void RecursiveSphere::subdivideTriangle(const glm::vec3 & v1, const glm::vec3 & v2, const glm::vec3 & v3, unsigned level)
{
	if (level == 0)  m_faces.push_back(Primitives::TriangleFace(v1, v2, v3, v1, v2, v3));
	else
	{
		glm::vec3 m1 = glm::normalize((v1 + v2) * 0.5f);
		glm::vec3 m2 = glm::normalize((v2 + v3) * 0.5f);
		glm::vec3 m3 = glm::normalize((v3 + v1) * 0.5f);
		subdivideTriangle(v1, m1, m3, level - 1);
		subdivideTriangle(m1, v2, m2, level - 1);
		subdivideTriangle(m3, m2, v3, level - 1);
		subdivideTriangle(m1, m2, m3, level - 1);
	}
}

void RecursiveSphere::draw(float objectTransparency, bool showNormals, float normalSize) const
{
	for (const auto & face : m_faces)
	{
		Rendering::glDrawTriangle(face, objectTransparency);

		if (showNormals)
		{
			Rendering::glDrawSegmentLine(Primitives::SegmentLine(face.v1, face.v1 + normalSize * face.n1));
			Rendering::glDrawSegmentLine(Primitives::SegmentLine(face.v2, face.v2 + normalSize * face.n2));
			Rendering::glDrawSegmentLine(Primitives::SegmentLine(face.v3, face.v3 + normalSize * face.n3));
		}
	}
}

Primitives::TriangleFace RecursiveSphere::getFace(unsigned index) const
{
	return m_faces[index];
}