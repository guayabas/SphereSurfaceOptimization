#define _CRT_SECURE_NO_WARNINGS
#include <nativefiledialog/nfd.h>
#include "PointDistribution.h"
#include <nlopt/api/nlopt.h>
#include "ReferenceTable.h"
#include "NLOptTesting.h"
#include "GUIInterface.h"
#include <QuickHull.hpp>
#include "Screenshoot.h"
#include "Parameteres.h"
#include "Animation.h"
#include "GUI.h"

static const float padding = 10.0f;
static const auto flags = (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

GUI::GUI(GLFWwindow * glfwwindow, Animation * animation, Parameters * parameters)
	: m_parameters(parameters)
	, m_glfwwindow(glfwwindow)
	, m_animation(animation)
{
	ImGui_ImplGlfw_Init(m_glfwwindow, true);
	ImGuiIO & io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
}

void GUI::lPanel(const float & width, const float & height)
{
	const auto & p = m_parameters;

	ImGui::SetNextWindowPos(ImVec2(padding, padding));
	ImGui::SetNextWindowSize(ImVec2(width, height - 2.0f * padding));

	static bool fixedOverlayOpened = true;
	if (!ImGui::Begin("Left Panel", &fixedOverlayOpened, ImVec2(), p->guiTransparency, flags)) { ImGui::End(); }

	ImGui::SliderFloat("GUI transparency", &p->guiTransparency, 0.0f, 1.0f);

	ImGui::InputInt("Sphere subdivision", &p->sphereSubdivision);
	if (p->sphereSubdivision < 0) p->sphereSubdivision = 0;
	if (p->sphereSubdivision > 5) p->sphereSubdivision = 5;

	ImGui::InputInt("GUI width ratio", &p->guiWidthRatio);
	if (p->guiWidthRatio < 1) p->guiWidthRatio = 1;
	if (p->guiWidthRatio > 9) p->guiWidthRatio = 9;

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Rendering", 0, true, false))
	{
		ImGui::SliderFloat("Material transparency", &p->materialTransparency, 0.0f, 1.0f);
		ImGui::SliderFloat4("Material ambient", &(p->materialAmbient[0]), 0.0f, 1.0f);
		ImGui::SliderFloat4("Material diffuse", &(p->materialDiffuse[0]), 0.0f, 1.0f);
		ImGui::SliderFloat3("Light position", &(p->lightPosition[0]), -2.0f, +2.0f);
		ImGui::SliderFloat4("Light ambient", &(p->lightAmbient[0]), 0.0f, 1.0f);
		ImGui::SliderFloat4("Light diffuse", &(p->lightDiffuse[0]), 0.0f, 1.0f);
		ImGui::SliderFloat("Normal scaling", &p->normalScaling, 0.0f, 1.0f);

		ImGui::Checkbox("Show initial distribution", &p->showInitialDistribution);
		ImGui::Checkbox("Polygon mode wire-frame", &p->polygonModeWireframe);
		ImGui::Checkbox("Show canonical circles", &p->showCanonicalCircles);
		ImGui::Checkbox("Polygon mode cull-face", &p->polygonModeCullface);
		ImGui::Checkbox("Show convex hull", &p->showConvexHull);
		ImGui::Checkbox("Enable lighting", &p->enableLighting);
		ImGui::Checkbox("Show normals", &p->showNormals);
		ImGui::Checkbox("Show sphere", &p->showSphere);
	}

	ImGui::Separator();
	nlopt_algorithm optimizationAlgorithmSelected = NLOPT_NUM_ALGORITHMS;
	static bool useMaxNumberIterations = true;
	static int maxNumberIterations = 100;
	static bool useGradient = true;
	if (ImGui::CollapsingHeader("Optimization", 0, true, true))
	{
		static bool useEqualityConstraint = false;
		static bool saveToFile = false;

		static int itemDistribution = 0;
		static int itemAlgorithm = 0;

		ImGui::Combo("Initial distribution", &itemDistribution, "UNIFORM\0RANDOM");
		ImGui::Combo("Algorithm", &itemAlgorithm, "LD_SLSQP\0LN_COBYLA");

		ImGui::InputInt("Maximum number iterations", &maxNumberIterations);
		ImGui::InputInt("Number particles", &p->numberParticles);

		ImGui::Checkbox("Use maximum number iterations", &useMaxNumberIterations);
		ImGui::Checkbox("Use equality constraint", &useEqualityConstraint);
		ImGui::Checkbox("Save iterations to file", &saveToFile);
		ImGui::Checkbox("Use gradient", &useGradient);

		if (itemAlgorithm == 1) optimizationAlgorithmSelected = NLOPT_LN_COBYLA;
		if (itemAlgorithm == 0) optimizationAlgorithmSelected = NLOPT_LD_SLSQP;

		if (p->numberParticles < 0x00000002) p->numberParticles = 0x00000002;
		if (maxNumberIterations > 10000) maxNumberIterations = 10000;
		if (maxNumberIterations < 10)  maxNumberIterations = 10;

		if (ImGui::Button("Solve optimization"))
		{
			p->pointCloud3dOptimizedCoordinates.clear();
			p->pointCloud3dInitialConditions.clear();

			PointDistribution::run_3d(
				p->pointCloud3dOptimizedCoordinates,
				p->pointCloud3dInitialConditions,
				optimizationAlgorithmSelected,
				(unsigned)maxNumberIterations,
				(unsigned)p->numberParticles,
				(unsigned)itemDistribution,
				useMaxNumberIterations,
				useEqualityConstraint,
				useGradient,
				saveToFile,
				true
			);
		}
	}

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Benchmarking", 0, true, true))
	{
		if (ImGui::Button("Benchmark distribution"))
		{
			std::vector<unsigned> x;
			std::vector<unsigned> y;
			std::vector<unsigned> z;

			FILE * handleFile = fopen("BenchmarkDistribution.txt", "w");

			std::vector<glm::vec3> noUse1;
			std::vector<glm::vec3> noUse2;
			for (unsigned numberParticles = 2; numberParticles <= 10; ++numberParticles)
			{
				std::cout << "running " << numberParticles << std::endl;

				auto solution1 = PointDistribution::run_3d
				(
					noUse1,
					noUse2,
					optimizationAlgorithmSelected,
					(unsigned)maxNumberIterations,
					numberParticles,
					0,
					useMaxNumberIterations,
					false,
					true,
					false,
					false
				);

				auto solution2 = PointDistribution::run_3d
				(
					noUse1,
					noUse2,
					optimizationAlgorithmSelected,
					(unsigned)maxNumberIterations,
					numberParticles,
					1,
					useMaxNumberIterations,
					false,
					true,
					false,
					false
				);

				x.push_back(numberParticles);
				y.push_back(solution1.first);
				z.push_back(solution2.first);

				fprintf
				(
					handleFile,
					"%d\n%d\n",
					solution1.first,
					solution2.first
				);
			}
			std::cout << "done\n";

			fclose(handleFile);
		}

		ImGui::SameLine();

		if (ImGui::Button("Benchmark algorithms"))
		{
			std::vector<glm::vec3> noUse1;
			std::vector<glm::vec3> noUse2;

			FILE * handleFile = fopen("BenchmarkAlgorithms.txt", "w");

			for (unsigned numberParticles = 2; numberParticles <= 50; ++numberParticles)
			{
				std::cout << "running " << numberParticles << std::endl;

				auto solution1 = PointDistribution::run_3d
				(
					noUse1,
					noUse2,
					NLOPT_LD_SLSQP,
					(unsigned)maxNumberIterations,
					numberParticles,
					0,
					useMaxNumberIterations,
					false,
					true,
					false,
					false
				);

				auto solution2 = PointDistribution::run_3d
				(
					noUse1,
					noUse2,
					NLOPT_LN_COBYLA,
					(unsigned)maxNumberIterations,
					numberParticles,
					0,
					useMaxNumberIterations,
					true,
					false,
					false,
					false
				);

				fprintf
				(
					handleFile,
					"%02d & %12.9f & %12.9f & %12.9f \\\\ \n",
					numberParticles,
					energyValuesThomson[numberParticles],
					solution1.second,
					solution2.second
				);
			}
			std::cout << "done\n";

			fclose(handleFile);
		}
	}

	ImGui::Separator();
	const auto maxNumberFrames = m_animation->getNumberFrames();
	if (ImGui::CollapsingHeader("Animation", 0, true, false))
	{
		ImGui::SliderFloat("Animation update rate", &p->animationUpdateRate, 0.0f, 1.0f);

		ImGui::InputInt("Selected frame to render", &p->animationFrameToRender);
		if (p->animationFrameToRender > (int)maxNumberFrames) p->animationFrameToRender = maxNumberFrames;
		if (p->animationFrameToRender < 0) p->animationFrameToRender = 0;

		if (ImGui::Button("Open file for animation"))
		{
			nfdchar_t * outputPath = nullptr;
			nfdresult_t result = NFD_OpenDialog("txt", nullptr, &outputPath);
			switch (result)
			{
			case NFD_OKAY: m_animation->readFrames(std::string(outputPath)); break;
			case NFD_CANCEL: default: break;
			case NFD_ERROR: break;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Toggle animation"))
		{ p->pauseAnimation ^= 1; }

		ImGui::Checkbox("Animate solution", &p->animateSolution);
	}

	ImGui::Separator();
	ImGui::Text("Eye coordinates world space %f %f %f", p->eye.x, p->eye.y, p->eye.z);
	ImGui::Text("Framebuffer size %d %d", p->windowWidth, p->windowHeight);
	ImGui::Text("Animation frame %d", m_animation->getCurrentFrame());
	ImGui::Text("Maximum frames %d", maxNumberFrames);

	ImGui::End();
}

void GUI::rPanel(const float & width, const float & height)
{
	const auto & p = m_parameters;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	ImGui::SetNextWindowPos(ImVec2(p->currentFrameWidth - width - padding, padding));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	static bool fixedOverlayOpened = true;
	if (!ImGui::Begin("Right Panel", &fixedOverlayOpened, ImVec2(), 1.0f, flags)) { ImGui::End(); }

	static bool includeGUI = false;
	ImGui::Checkbox("Include gui", &includeGUI);
	if (ImGui::Button("Take screen-shoot")) screenshoot("Sample", p->currentFrameWidth, p->currentFrameHeight, includeGUI);
	ImGui::SameLine();
	if (ImGui::Button("Reset parameters")) p->reset();

	ImGui::End();
	ImGui::PopStyleColor(1);
}

void GUI::draw()
{
	ImGui_ImplGlfw_NewFrame();
	lPanel(m_parameters->currentFrameWidth / float(m_parameters->guiWidthRatio), (float)m_parameters->currentFrameHeight);
	rPanel(350.0f, 62.5f);
	ImGui::Render();
}