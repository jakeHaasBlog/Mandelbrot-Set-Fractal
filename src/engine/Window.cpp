#include "engine/Window.h"
#include <chrono>

#include "game/GameLogicInterface.h"

#include "engine/ViewportManager.h"

Window & Window::getWindowInstance() {
	static Window w;
	return w;
}

Window::Window() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_MAXIMIZED, true);
	windowHandle = glfwCreateWindow(1920, 1080, "Ray Casting Project", NULL, NULL);
	glfwMakeContextCurrent(windowHandle);
	glfwSwapInterval(1);

	glfwSetKeyCallback(windowHandle, keyCallback);
	glfwSetCharCallback(windowHandle, charCallback);
	glfwSetMouseButtonCallback(windowHandle, mouseButtonCallback);
	glfwSetCursorPosCallback(windowHandle, mouseMoveCallback);
}

void Window::mainUpdateLoop() {

	std::chrono::high_resolution_clock clock;
	std::chrono::time_point<std::chrono::steady_clock> start = clock.now();
	std::chrono::time_point<std::chrono::steady_clock> end = clock.now();

	std::chrono::time_point<std::chrono::steady_clock> startUpdateTIme = clock.now();
	float updateTime = 0.0f;
	float deltaTime = 0.0f;
	while (!glfwWindowShouldClose(windowHandle)) {
		glfwSwapInterval(1);
		glfwSwapBuffers(windowHandle);

		startUpdateTIme = clock.now();

		glfwPollEvents();

		end = clock.now();
		deltaTime = (float)((float)(end - start).count() / 1000000.0f);
		start = clock.now();

		//framebuffer.bindAsRenderTarget();
		GameLogicInterface::update(deltaTime);
		//framebuffer.unbindAsRenderTarget();

		updateTime = (float)(((double)(clock.now() - startUpdateTIme).count()) / 1000000.0f);

		ViewportManager::update();
		glfwMakeContextCurrent(windowHandle);

		static uint64_t frame = 0;
		frame++;
		if (frame % 10 == 0) {
			calculateFPS();

			char t[256];
			sprintf_s(t, "FPS: %.1f - delta time(%.2fms) - update time(%fms)", (float)getFrameRate(), deltaTime, updateTime);
			setTitle(t);
		}
	}

}

void Window::setSize(int width, int height) {
	glfwSetWindowSize(windowHandle, width, height);
}

void Window::setTitle(const std::string & title) {
	this->title = title;
	glfwSetWindowTitle(windowHandle, this->title.c_str());
}

int Window::getWidth() {
	int width;
	glfwGetWindowSize(windowHandle, &width, nullptr);
	return width;
}

int Window::getHeight() {
	int height;
	glfwGetWindowSize(windowHandle, nullptr, &height);
	return height;
}

std::string Window::getTitle() {
	return title;
}

int Window::getFrameBufferWidth()
{
	return framebuffer.getWidth();
}

int Window::getFrameBufferHeight()
{
	return framebuffer.getHeight();
}

float Window::getAspectRatio() {
	int width, height;
	glfwGetWindowSize(windowHandle, &width, &height);
	return (float)width / height;
}

double Window::getFrameRate(){
	return fps;
}

bool Window::keyIsDown(int glfwKey) {
	int state = glfwGetKey(windowHandle, glfwKey);
	return state == GLFW_PRESS;
}

float Window::getMouseX() {
	return mouseX;
}

float Window::getMouseY() {
	return mouseY;
}

void Window::close() {
	glfwSetWindowShouldClose(windowHandle, true);
}

GLFWwindow * Window::getHandle() {
	return windowHandle;
}

void Window::calculateFPS() {
	static std::chrono::high_resolution_clock clock;
	static long long timeA;
	static long long timeB;
	timeA = clock.now().time_since_epoch().count();

	long long deltaTime = timeA - timeB;

	timeB = clock.now().time_since_epoch().count();

	fps =  1.0f / ((double)(deltaTime / 10) / 1000000000);
}

void Window::setResolution(int width, int height)
{
	float* data = new float[width * height * 4];
	framebuffer.generateFromData(width, height, data, width * height);
	delete[] data;
}

void Window::keyCallback(GLFWwindow * wind, int key, int scancode, int action, int mods) {
	GameLogicInterface::keyCallback(key, scancode, action, mods);
}

void Window::charCallback(GLFWwindow * wind, unsigned int codepoint) {
	GameLogicInterface::characterCallback(codepoint);
}

void Window::mouseButtonCallback(GLFWwindow * wind, int button, int action, int mods) {
	GameLogicInterface::mouseButtonCallback(button, action, mods);
}

void Window::mouseMoveCallback(GLFWwindow * wind, double xPos, double yPos) {
	int width, height;
	glfwGetWindowSize(window.windowHandle, &width, &height);
	window.mouseX = ((float)(xPos - width / 2) / (width / 2)) * window.getAspectRatio();
	window.mouseY = (float)(-yPos + height / 2) / (height / 2);
	GameLogicInterface::mouseMoveCallback(xPos, yPos);
}
