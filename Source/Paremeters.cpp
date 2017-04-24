#include "Parameteres.h"

Parameters::Parameters()
{
	initialvalues();
}

void Parameters::initialvalues()

{
	eye = glm::vec3(0.0f);

	materialAmbient[0] = 0.2f;
	materialAmbient[1] = 0.2f;
	materialAmbient[2] = 0.2f;
	materialAmbient[3] = 1.0f;
	materialDiffuse[0] = 0.8f;
	materialDiffuse[1] = 0.8f;
	materialDiffuse[2] = 0.8f;
	materialDiffuse[3] = 1.0f;
	lightPosition[0] = 0.0f;
	lightPosition[1] = 0.0f;
	lightPosition[2] = 2.0f;
	lightPosition[3] = 1.0f;
	lightAmbient[0] = 0.2f;
	lightAmbient[1] = 0.2f;
	lightAmbient[2] = 0.2f;
	lightAmbient[3] = 1.0f;
	lightDiffuse[0] = 0.8f;
	lightDiffuse[1] = 0.8f;
	lightDiffuse[2] = 0.8f;
	lightDiffuse[3] = 1.0f;

	materialTransparency = 0.1f;
	animationUpdateRate = 0.0f;
	guiTransparency = 0.5f;
	normalScaling = 1.0f;
	translateX = +0.0f;
	translateY = +0.0f;
	translateZ = -5.0f;
	angleX = 0.0f;
	angleY = 0.0f;

	animationFrameToRender = -1;
	currentCursorPositionX = -1;
	currentCursorPositionY = -1;
	currentFrameHeight = -1;
	currentFrameWidth = -1;	
	sphereSubdivision = 5;
	numberParticles = 2;
	windowHeight = 1200;
	guiWidthRatio = 2;
	windowWidth = 1200;

	showInitialDistribution = true;
	showCanonicalCircles = false;
	windowFullscreenMode = false;
	polygonModeWireframe = false;
	polygonModeCullface = true;
	mouseButtonPressed = false;
	finishProgramFlag = false;
	modifiersPressed = false;
	animateSolution = false;
	enableLighting = false;
	pauseAnimation = false;
	showConvexHull = true;
	showNormals = false;
	showSphere = true;
}

void Parameters::reset()
{
	initialvalues();
}