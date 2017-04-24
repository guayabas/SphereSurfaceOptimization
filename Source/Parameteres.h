#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

struct Parameters
{
	Parameters();
	inline Parameters * get() { return this; }

	void reset();

	std::vector<glm::vec2> pointCloud2dOptimizedCoordinates;
	std::vector<glm::vec3> pointCloud3dOptimizedCoordinates;
	std::vector<glm::vec2> pointCloud2dInitialConditions;
	std::vector<glm::vec3> pointCloud3dInitialConditions;
	glm::vec3 eye;

	float materialAmbient[4];
	float materialDiffuse[4];
	float lightPosition[4];
	float lightAmbient[4];
	float lightDiffuse[4];

	float materialTransparency;
	float animationUpdateRate;
	float guiTransparency;
	float normalScaling;
	float translateX;
	float translateY;
	float translateZ;
	float angleX;
	float angleY;

	int animationFrameToRender;
	int currentCursorPositionX;
	int currentCursorPositionY;
	int currentFrameHeight;
	int currentFrameWidth;
	int sphereSubdivision;
	int numberParticles;
	int guiWidthRatio;
	int windowHeight;
	int windowWidth;

	bool showInitialDistribution;
	bool windowFullscreenMode;
	bool polygonModeWireframe;
	bool showCanonicalCircles;
	bool polygonModeCullface;
	bool mouseButtonPressed;
	bool rightButtonPressed;
	bool finishProgramFlag;
	bool modifiersPressed;
	bool animateSolution;
	bool enableLighting;
	bool showConvexHull;
	bool pauseAnimation;
	bool showNormals;
	bool showSphere;

protected:
	void initialvalues();
};