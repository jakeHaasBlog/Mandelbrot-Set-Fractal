#include "engine/ViewportManager.h"

#include <intrin.h>
#include "Window.h"

std::stack<std::array<int, 4>> ViewportManager::viewportHistory;
std::array<float, 4> ViewportManager::viewportBound;

void ViewportManager::bindViewportNormalized(float x, float y, float width, float height)
{
	float aspectRatio = window.getAspectRatio();

	float left = x / aspectRatio;
	float right = (x + width) / aspectRatio;
	float top = y + height;
	float bottom = y;

	int pX = (left * window.getWidth()) / 2 + window.getWidth() / 2;
	int pY = (bottom * window.getHeight()) / 2 + window.getHeight() / 2;
	int pW = ((right - left) / 2) * window.getWidth();
	int pH = ((top - bottom) / 2) * window.getHeight();

	glViewport(pX, pY, pW, pH);
	viewportHistory.push({ pX, pY, pW, pH });

	calculateViewportBound();

}

void ViewportManager::bindViewportPixels(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
	viewportHistory.push({ x, y, width, height });

	calculateViewportBound();
}

void ViewportManager::unbindViewport()
{
	viewportHistory.pop();
	glViewport(viewportHistory.top()[0], viewportHistory.top()[1], viewportHistory.top()[2], viewportHistory.top()[3]);

	calculateViewportBound();
}

std::array<float, 4> ViewportManager::getViewportNormalized()
{ 
	//int pX = (left * window.getWidth()) / 2 + window.getWidth() / 2;      ----->  left = (2pX - window.getWidth()) / window.getWidth()
	//int pY = (bottom * window.getHeight()) / 2 + window.getHeight() / 2;  ----->  bottom = (2pY - window.getHeight()) / window.getHeight()
	//int pW = ((right - left) / 2) * window.getWidth();                    ----->  right - left = (pW / window.getWidth()) * 2
	//int pH = ((top - bottom) / 2) * window.getHeight();                   ----->  top - bottom = (pH / window.getHeight()) * 2

	int pX = viewportHistory.top()[0];
	int pY = viewportHistory.top()[1];
	int pW = viewportHistory.top()[2];
	int pH = viewportHistory.top()[3];

	float left = left = (2 * pX - window.getWidth()) / window.getWidth();
	float bottom = (2 * pY - window.getHeight()) / window.getHeight();
	float right = (pW / window.getWidth()) * 2;
	float top = (pH / window.getHeight()) * 2;

	//float left = x / aspectRatio;                -----> x = left * aspectRatio
	//float right = (x + width) / aspectRatio;     -----> width = right * aspectRatio - x
	//float top = y + height;                      -----> height = top - y
	//float bottom = y;                            -----> bottom = y

	float aspectRatio = window.getAspectRatio();

	float x = left * aspectRatio;
	float y = bottom;
	float width = right * aspectRatio - x;
	float height = top - y;

	return { x, y, width, height };
}

std::array<int, 4> ViewportManager::getViewportPixels()
{
	return viewportHistory.top();
}

float ViewportManager::getCurrentAspectRatio()
{
	if (viewportHistory.size() <= 1) {
		return window.getAspectRatio();
	}

	return (float)viewportHistory.top()[2] / (float)viewportHistory.top()[3];
}

float ViewportManager::getLeftViewportBound()
{
	return viewportBound[0];
}

float ViewportManager::getRightViewportBound()
{
	return viewportBound[1];
}

float ViewportManager::getTopViewportBound()
{
	return viewportBound[2];
}

float ViewportManager::getBottomViewportBound()
{
	return viewportBound[3];
}

void ViewportManager::init() {
	static bool isInitialized = false;
	if (isInitialized) {
		__debugbreak(); // this class was initilized twice
	}

	bindViewportPixels(0, 0, window.getWidth(), window.getHeight());

	isInitialized = true;
}

void ViewportManager::update()
{
	if (viewportHistory.size() > 1) {
		__debugbreak(); // there were 1 or more viewports that were not unbound this frame
	}

	viewportHistory.top() = { 0, 0, window.getWidth(), window.getHeight() };
	glViewport(viewportHistory.top()[0], viewportHistory.top()[1], viewportHistory.top()[2], viewportHistory.top()[3]);
}

void ViewportManager::calculateViewportBound()
{
	float aspectRatio = getCurrentAspectRatio();
	viewportBound = { -1 * aspectRatio,  1 * aspectRatio, 1.0f, -1.0f};
}
