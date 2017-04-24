#include <GLFW\glfw3.h>
#include "Renderingh.h"
#include "Animation.h"
#include <iostream>
#include <fstream>

Animation::Animation()
	: m_currentFrame(0)
{

}

Animation::~Animation()
{
	clearFrames();
}

void Animation::clearFrames()
{
	m_frames.clear();
}

void Animation::readFrames(const std::string & fileName)
{
	clearFrames();

	std::ifstream handleFileToOpen(fileName);
	if (handleFileToOpen.is_open())
	{
		std::string currentLine;
		std::vector<double> currentFrame;

		while (getline(handleFileToOpen, currentLine))
		{
			if (currentLine[0] == 'I')
			{
				m_frames.push_back(currentFrame);
				currentFrame.clear();
			}
			else
			{
				currentFrame.push_back(std::stod(currentLine, nullptr));
			}
		}

		handleFileToOpen.close();
	}

	m_maxNumberFrames = (unsigned)m_frames.size();
}

unsigned Animation::getCurrentFrame() const
{
	return (unsigned)m_currentFrame;
}

unsigned Animation::getNumberFrames() const
{
	return m_maxNumberFrames;
}

void Animation::renderFrame(unsigned frame)
{
	if (frame > (m_maxNumberFrames - 1)) return;

	std::vector<glm::vec3> pointCloud;
	const size_t numberCoordinates = m_frames[frame].size();
	for (size_t i = 0; i < numberCoordinates; i += 3)
	{
		double x = m_frames[frame][i + 0];
		double y = m_frames[frame][i + 1];
		double z = m_frames[frame][i + 2];
		pointCloud.push_back(glm::vec3(x, y, z));
	}
	Rendering::glDrawConvexHull(pointCloud);
}

void Animation::renderFrames()
{
	renderFrame((unsigned)m_currentFrame);
}

void Animation::updateFrame()
{
	if (++m_currentFrame > (m_maxNumberFrames - 1)) m_currentFrame = 0;
}

Animation * Animation::get()
{
	return this;
}