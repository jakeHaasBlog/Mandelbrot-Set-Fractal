#pragma once

#include <array>
#include <stack>

class ViewportManager {
public:
	// not adjusted for window aspect ratio, MUST UNBIND AFTER RENDERING
	static void bindViewportNormalized(float x, float y, float width, float height);

	// not adjusted for aspect ratio, MUST UNBIND AFTER RENDERING
	static void bindViewportPixels(int x, int y, int width, int height);

	static void unbindViewport();

	// adjusted for window aspect ratio
	static std::array<float, 4> getViewportNormalized();
	static std::array<int, 4> getViewportPixels();

	static float getCurrentAspectRatio();

	static float getLeftViewportBound();
	static float getRightViewportBound();
	static float getTopViewportBound();
	static float getBottomViewportBound();


public:
	// sets the base viewport to the window
	static void init(); // dont touch me! >:(

	// this function is called at the end of every frame to ensure all bound viewports are unbound and accounts for window resizing
	static void update(); // dont touch me! D:<

private:
	static std::stack<std::array<int, 4>> viewportHistory;

	static void calculateViewportBound();
	static std::array<float, 4> viewportBound;

};