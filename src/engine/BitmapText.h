#pragma once

#include "engine/BitmapFont.h"
#include "engine/BatchQuads.h"
#include "engine/FontManager.h"
#include <string>
#include <array>

class BitmapText {
public:
	BitmapText(BitmapFont* font);
	BitmapText();

	void setText(const std::string& text);
	const std::string& getText();

	void setColor(float r, float g, float b, float a = 1.0f);
	void setColor(const std::array<float, 4>& color);
	float getR();
	float getG();
	float getB();

	void setBackgroundColor(float r, float g, float b, float a = 1.0f);
	void setBackgroundColor(const std::array<float, 4>& color);
	float getBackgroundR();
	float getBackgroundG();
	float getBackgroundB();
	float getBackgroundA();
	
	// only render the first x characters
	void setCharacterRenderCount(int x);
	int getCharacterRenderCount();
	void resetRenderCount();

	void setPosition(const std::array<float, 2>& pos);
	void setPosition(float x, float y);
	float getX();
	float getY();

	void setCharHeight(float height);
	float getCharHeight();

	void setFont(BitmapFont* font);
	BitmapFont* getFont();

	void render();

private:
	std::array<float, 2> position;
	std::array<float, 4> color;
	std::array<float, 4> bgColor;
	float charHeight = 0.01f;
	int renderCount;

	BitmapFont* font;
	BatchQuads bq;

	std::string text = "default text";

	bool textHasChanged = true;

	static Shader textShader;

};


