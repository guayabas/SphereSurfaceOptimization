#pragma once

struct Parameters;
struct GLFWwindow;
class Animation;

class GUI
{
public:
	GUI(GLFWwindow *, Animation * = nullptr, Parameters * = nullptr);

	void draw();
	void exit();

protected:
	void lPanel(const float &, const float &);
	void rPanel(const float &, const float &);

private:
	Parameters * m_parameters;
	GLFWwindow * m_glfwwindow;
	Animation * m_animation;
};