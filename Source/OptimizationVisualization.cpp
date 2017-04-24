#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RecursiveSphere.h"
#include "Parameteres.h"
#include <GLFW/glfw3.h>
#include "Renderingh.h"
#include "Animation.h"
#include <iostream>
#include <imgui.h>
#include <memory>
#include <ctime>
#include "GUI.h"

static std::unique_ptr<RecursiveSphere> recursivesphere;
static std::unique_ptr<Parameters> parameters;
static std::unique_ptr<Animation> animation;
static std::unique_ptr<GUI> gui;

static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int modifiers)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_Q: parameters->finishProgramFlag = true; break;
		case GLFW_KEY_F:
		{
			parameters->windowFullscreenMode ^= 1;
			const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			if (parameters->windowFullscreenMode) glfwSetWindowSize(window, mode->width, mode->height);
			else glfwSetWindowSize(window, parameters->windowWidth, parameters->windowHeight);
		} break;
		default: break;
		}
	}
}

static void mouseCallback(GLFWwindow * window, int button, int action, int modifier)
{
	switch (modifier)
	{
	case GLFW_MOD_CONTROL:
	case GLFW_MOD_SHIFT:
	case GLFW_MOD_ALT:
		parameters->modifiersPressed = true;
		break;

	default:
		parameters->modifiersPressed = false;
		break;
	}

	if (action == GLFW_PRESS)
	{
		parameters->mouseButtonPressed = true;
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			double cursorPositionx;
			double cursorPositiony;
			glfwGetCursorPos(window, &cursorPositionx, &cursorPositiony);
			parameters->currentCursorPositionX = (int)cursorPositionx;
			parameters->currentCursorPositionY = (int)cursorPositiony;

			parameters->rightButtonPressed = false;
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			parameters->rightButtonPressed = true;
	}
	else parameters->mouseButtonPressed = false;
}

static void motionCallback(GLFWwindow * window, double xposition, double yposition)
{
	if (ImGui::IsMouseHoveringAnyWindow()) return;
	int dx = static_cast<int>(parameters->currentCursorPositionX - xposition);
	int dy = static_cast<int>(parameters->currentCursorPositionY - yposition);
	if (parameters->mouseButtonPressed)
	{
		if (parameters->modifiersPressed)
		{
			parameters->translateX -= (0.05f * dx);
			parameters->translateY += (0.05f * dy);
		}
		else
		{
			if (parameters->rightButtonPressed) parameters->translateZ -= (0.1f * dx);
			else
			{
				parameters->angleY -= dx;
				if (parameters->angleY > 360.0f || parameters->angleY < -360.0f) parameters->angleY = 0.0f;
				parameters->angleX -= dy;
				if (parameters->angleX > 360.0f || parameters->angleX < -360.0f) parameters->angleX = 0.0f;
			}			
		}
	}
	parameters->currentCursorPositionX = (int)xposition;
	parameters->currentCursorPositionY = (int)yposition;
}

static void scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	parameters->translateZ += static_cast<int>(yoffset);
}

int main(int argc, char ** argv)
{
	srand((unsigned)time(NULL));

	static GLFWwindow * glfwwindow = nullptr;
	parameters = std::make_unique<Parameters>();

	if (glfwInit() != GLFW_TRUE) return -1;
	glfwWindowHint(GLFW_DECORATED, 1);
	glfwWindowHint(GLFW_SAMPLES, 16);

	glfwwindow = glfwCreateWindow(parameters->windowWidth, parameters->windowHeight, "", nullptr, nullptr);
	if (!glfwwindow) { glfwTerminate(); return -1; }

	recursivesphere = std::make_unique<RecursiveSphere>();
	animation = std::make_unique<Animation>();

	glfwMakeContextCurrent(glfwwindow);
	gui = std::make_unique<GUI>(glfwwindow, animation->get(), parameters->get());


	glfwSetMouseButtonCallback(glfwwindow, mouseCallback);
	glfwSetCursorPosCallback(glfwwindow, motionCallback);
	glfwSetScrollCallback(glfwwindow, scrollCallback);
	glfwSetKeyCallback(glfwwindow, keyCallback);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(glfwwindow) && !parameters->finishProgramFlag)
	{
		recursivesphere->recompute(parameters->sphereSubdivision);

		glfwPollEvents();

		glfwGetWindowSize(glfwwindow, &parameters->currentFrameWidth, &parameters->currentFrameHeight);
		glViewport(0, 0, parameters->currentFrameWidth, parameters->currentFrameHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		const auto aspectRatio = parameters->currentFrameWidth / float(parameters->currentFrameHeight);
		const auto projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);
		glLoadMatrixf(glm::value_ptr(projection));

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		const auto translate = glm::translate(glm::vec3(parameters->translateX, parameters->translateY, parameters->translateZ));
		const auto rotatex = glm::rotate(glm::radians(parameters->angleX), glm::vec3(1.0f, 0.0f, 0.0f));
		const auto rotatey = glm::rotate(glm::radians(parameters->angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		const auto view = (translate * rotatex * rotatey);
		parameters->eye = glm::vec3(rotatey * rotatex * translate * glm::vec4(glm::vec3(0.0f), 1.0f));
		glLoadMatrixf(glm::value_ptr(view));

		(parameters->polygonModeWireframe) 
			? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) 
			: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		(parameters->polygonModeCullface) 
			? glEnable(GL_CULL_FACE) 
			: glDisable(GL_CULL_FACE);

		static double animationTime = 0.0;
		static double lastTime = 0.0;
		
		double currentTime = glfwGetTime();
		double updateTime = (currentTime - lastTime);
		lastTime = currentTime;

		animationTime += updateTime;
		if (animationTime > parameters->animationUpdateRate)
		{
			if (parameters->animateSolution && !parameters->pauseAnimation) 
				animation->updateFrame();
			
			animationTime = 0.0;
		}

		if (parameters->pauseAnimation) animation->renderFrame(parameters->animationFrameToRender);
		if (parameters->animateSolution) animation->renderFrames();

		Rendering::glDisableLight();

		if (parameters->showCanonicalCircles)
		{
			Rendering::glDrawCircle(0);
			Rendering::glDrawCircle(1);
			Rendering::glDrawCircle(2);
		}

		if (parameters->enableLighting)
		{
			Rendering::glEnableLight();
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, parameters->materialAmbient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, parameters->materialDiffuse);
			glLightfv(GL_LIGHT0, GL_POSITION, parameters->lightPosition);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, parameters->lightDiffuse);
			glLightfv(GL_LIGHT0, GL_AMBIENT, parameters->lightAmbient);
		}

		if (parameters->showInitialDistribution)
		{
			Rendering::glDrawPointCloud(parameters->pointCloud3dInitialConditions, glm::vec3(0.0f, 0.0f, 1.0f), true);
		}

		if (parameters->showConvexHull)
		{
			Rendering::glDrawConvexHull(parameters->pointCloud3dOptimizedCoordinates);
		}

		if (parameters->showSphere)
		{
			recursivesphere->draw(parameters->materialTransparency, parameters->showNormals, parameters->normalScaling);
		}
		
		Rendering::glDisableLight();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		gui->draw();

		glfwSwapBuffers(glfwwindow);
	}

	glfwDestroyWindow(glfwwindow); glfwTerminate(); return 0;
}