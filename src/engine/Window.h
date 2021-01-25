#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>
#include <array>
#include <stack>

#include "engine/Texture.h"

class Window {
public:
	static Window& getWindowInstance();

private:
	Window();

public:

	void mainUpdateLoop();

	void setSize(int width, int height);
	void setTitle(const std::string& title);
	void setResolution(int width, int height);

	int getWidth();
	int getHeight();
	std::string getTitle();

	int getFrameBufferWidth();
	int getFrameBufferHeight();

	float getAspectRatio();

	// based on the time between actually seeing frames, v-sync caps this to 60FPS
	double getFrameRate();

	bool keyIsDown(int glfwKey);

	float getMouseX();
	float getMouseY();

	void close();

	GLFWwindow* getHandle();

private:

	std::string title;
	GLFWwindow* windowHandle;

	void calculateFPS(); // called every exactly 10 frames 
	float fps;

	float mouseX, mouseY;
	static void keyCallback(GLFWwindow* wind, int key, int scancode, int action, int mods);
	static void charCallback(GLFWwindow* wind, unsigned int codepoint);
	static void mouseButtonCallback(GLFWwindow* wind, int button, int action, int mods);
	static void mouseMoveCallback(GLFWwindow* wind, double xPos, double yPos);

	Texture framebuffer;
};

#define window Window::getWindowInstance()