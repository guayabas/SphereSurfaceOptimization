#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include <string>

class Animation
{
public:
	Animation(); ~Animation();

	void readFrames(const std::string &);

	unsigned getCurrentFrame() const;
	unsigned getNumberFrames() const;

	void renderFrame(unsigned);
	void renderFrames();
	void updateFrame();

	Animation * get();

protected:
	void clearFrames();

private:
	std::vector<std::vector<double>> m_frames;
	unsigned m_maxNumberFrames;
	size_t m_currentFrame;
};