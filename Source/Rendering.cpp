#include <quickhull/QuickHull.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "Renderingh.h"

namespace Rendering
{
	static inline double interpolate(double val, double y0, double x0, double y1, double x1) 
	{
		return ((val - x0) * (y1 - y0) / (x1 - x0)) + y0;
	}

	static inline double base(double val) 
	{
		if		(val <= -0.75) return 0.0;
		else if (val <= -0.25) return interpolate(val, 0.0, -0.75, 1.0, -0.25);
		else if (val <= +0.25) return 1.0;
		else if (val <= +0.75) return interpolate(val, 1.0, +0.25, 0.0, +0.75);
		else return 0.0;
	}

	namespace ColorGradient
	{
		static inline double r(double gray)
		{
			return base(gray - 0.5);
		}

		static inline double g(double gray)
		{
			return base(gray);
		}

		static inline double b(double gray)
		{
			return base(gray + 0.5);
		}

		static const glm::uvec3 colorGradientTable[10] =
		{
			glm::uvec3(255, 0x0, 0x0),
			glm::uvec3(255, 140, 0x0),
			glm::uvec3(0x0, 100, 0x0),
			glm::uvec3(0x0, 139, 139),
			glm::uvec3(0x0, 255, 255),
			glm::uvec3(0x0, 191, 255),
			glm::uvec3(135, 206, 250),
			glm::uvec3(0x0, 0x0, 128),
			glm::uvec3(139, 0x0, 139),
			glm::uvec3(255, 182, 193)
		};
	}

	static inline glm::vec3 convertoglmvector(const quickhull::Vector3<float> vector)
	{
		return glm::vec3(vector.x, vector.y, vector.z);
	}

	static std::vector<quickhull::Vector3<float>> convertoquickhullcontainer(const std::vector<glm::vec3> & glmcontainer)
	{
		std::vector<quickhull::Vector3<float>> returningType; returningType.reserve(glmcontainer.size());
		for (const auto & point : glmcontainer)
		{ returningType.push_back(quickhull::Vector3<float>(point.x, point.y, point.z)); }
		return returningType;
	}

	void glDrawPoint(const Point & point, const float & pointsize)
	{
		glPointSize(pointsize);
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex3fv(glm::value_ptr(point.v1));
		glEnd();
		glPointSize(1.0f);
	}

	void glDrawSegmentLine(const SegmentLine & line, const float & linewidth)
	{
		glLineWidth(linewidth);
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3fv(glm::value_ptr(line.v1));
		glVertex3fv(glm::value_ptr(line.v2));
		glEnd();
		glLineWidth(1.0f);
	}

	void glDrawTriangle(const TriangleFace & face, const float & transparency)
	{
		glColor4f(0.0f, 0.0f, 0.0f, transparency);
		glBegin(GL_TRIANGLES);
		glNormal3fv(glm::value_ptr(face.n1));
		glVertex3fv(glm::value_ptr(face.v1));
		glNormal3fv(glm::value_ptr(face.n2));
		glVertex3fv(glm::value_ptr(face.v2));
		glNormal3fv(glm::value_ptr(face.n3));
		glVertex3fv(glm::value_ptr(face.v3));
		glEnd();
	}

	void glDrawCircle(unsigned axis)
	{
		constexpr unsigned numberPoints = 100;
		constexpr float inverseNumberPoints = (1.0f / float(numberPoints));

		std::vector<glm::vec3> points;
		glm::vec3 color = glm::vec3(0.0f);
		for (unsigned n = 0; n < numberPoints; ++n)
		{
			float normalizedValue = (n * inverseNumberPoints);
			float normalizedAngle = (glm::two_pi<float>() * normalizedValue);

			constexpr float radius = 1.005f;
			glm::vec3 point(0.0f);
			switch (axis)
			{
			case 0: 
				point = radius * glm::vec3(0.0f, glm::cos(normalizedAngle), glm::sin(normalizedAngle));
				color = glm::vec3(1.0f, 1.0f, 0.0f);
				break;
			case 1: 
				point = radius * glm::vec3(glm::cos(normalizedAngle), 0.0f, glm::sin(normalizedAngle));
				color = glm::vec3(1.0f, 0.0f, 1.0f);
				break;
			case 2: 
				point = radius * glm::vec3(glm::cos(normalizedAngle), glm::sin(normalizedAngle), 0.0f);
				color = glm::vec3(0.0f, 1.0f, 1.0f);
				break;
			default: break;
			}

			points.push_back(point);
		}

		glLineWidth(3.0f);
		glColor3fv(glm::value_ptr(color));
		glBegin(GL_LINE_LOOP);
		for (const auto & point : points) glVertex3fv(glm::value_ptr(point));
		glEnd();
		glLineWidth(1.0f);
	}

	void glDrawConvexHull(const std::vector<glm::vec3> & pointcloud)
	{
		if (!pointcloud.empty())
		{
			quickhull::QuickHull<float> quickhull;
			auto hull = quickhull.getConvexHull(convertoquickhullcontainer(pointcloud), true, false);
			auto vertexBuffer = hull.getVertexBuffer();
			auto indexBuffer = hull.getIndexBuffer();

			glLineWidth(5.0f);
			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_LINES);
			for (size_t i = 0; i < indexBuffer.size(); i += 3)
			{
				auto v1 = vertexBuffer[indexBuffer[i + 0]];
				auto v2 = vertexBuffer[indexBuffer[i + 1]];
				auto v3 = vertexBuffer[indexBuffer[i + 2]];
				glVertex3f(v1.x, v1.y, v1.z);
				glVertex3f(v2.x, v2.y, v2.z);
				glVertex3f(v2.x, v2.y, v2.z);
				glVertex3f(v3.x, v3.y, v3.z);
				glVertex3f(v3.x, v3.y, v3.z);
				glVertex3f(v1.x, v1.y, v1.z);
			}
			glEnd();
			glLineWidth(1.0f);

			glBegin(GL_TRIANGLES);
			unsigned colorIndex = 0;
			for (size_t i = 0; i < indexBuffer.size(); i += 3)
			{
				size_t i0 = indexBuffer[i + 0];
				size_t i1 = indexBuffer[i + 1];
				size_t i2 = indexBuffer[i + 2];

				if (i0 == i1 || i0 == i2 || i1 == i2) continue;

				auto v1 = vertexBuffer[i0];
				auto v2 = vertexBuffer[i1];
				auto v3 = vertexBuffer[i2];
				auto n1 = glm::normalize(glm::cross(convertoglmvector(v2 - v1), convertoglmvector(v3 - v1)));
				auto n2 = glm::normalize(glm::cross(convertoglmvector(v1 - v2), convertoglmvector(v3 - v2)));
				auto n3 = glm::normalize(glm::cross(convertoglmvector(v1 - v3), convertoglmvector(v2 - v3)));

				double gray = ((i / double(indexBuffer.size() - 1)));
				glColor4d(ColorGradient::r(gray), ColorGradient::g(gray), ColorGradient::b(gray), 0.8);

				glNormal3fv(glm::value_ptr(n1));
				glVertex3f(v1.x, v1.y, v1.z);
				glNormal3fv(glm::value_ptr(n3));
				glVertex3f(v3.x, v3.y, v3.z);
				glNormal3fv(glm::value_ptr(n2));
				glVertex3f(v2.x, v2.y, v2.z);
			}
			glEnd();
		}			
	}

	void glDrawPointCloud(const std::vector<glm::vec3> & points, const glm::vec3 & color, bool inverseColors)
	{
		glPointSize(15.0f);
		glBegin(GL_POINTS);
		for (size_t i = 0; i < points.size(); ++i)
		{
			double gray = ((i / double(points.size() - 1)));
			glColor3d(ColorGradient::r(gray), ColorGradient::g(gray), ColorGradient::b(gray));
			glVertex3f(points[i].x, points[i].y, points[i].z);
		}
		glEnd();
		glPointSize(1.0f);
	}

	void glDisableLight()
	{
		glDisable(GL_LIGHT0); glDisable(GL_LIGHTING);
	}

	void glEnableLight()
	{
		glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
	}
}