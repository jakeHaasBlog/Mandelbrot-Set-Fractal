
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "engine/Window.h"
#include "engine/ViewportManager.h"
#include "engine/FontManager.h"
#include "engine/TextureManager.h"
#include "game/GameLogicInterface.h"


int main(int argc, char** argv) {
	if (!glfwInit()) printf("GLFW did not initialize properly\n");
	window; // calls the constructor of window and loads it into static memory
	if (glewInit() != GLEW_OK) printf("GLEW did not initialize properly\n");

	FontManager::init();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.setResolution(1920, 1080);
	ViewportManager::init();
	GameLogicInterface::init();
	window.mainUpdateLoop();
	GameLogicInterface::cleanup();
	FontManager::cleanup();
	TextureManager::cleanup();

	glfwDestroyWindow(window.getHandle());
	glfwTerminate();
}