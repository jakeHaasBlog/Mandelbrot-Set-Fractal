#include "engine/BitmapText.h"

#include "engine/BatchQuads.h"

namespace {
	std::string vsString = 
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec2 position;\n"
		"layout(location = 1) in vec2 uvCoord;\n"
		"layout(location = 2) in vec4 color;\n"
		"layout(location = 3) in float tex\n;"
		"\n"
		"out vec2 v_uvCoord;\n"
		"out vec4 v_color;\n"
		"out float v_tex;\n"
		"\n"
		"uniform float u_aspectRatio;\n"
		"uniform float u_stretch;\n"
		"uniform vec2 u_translation;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4((position[0] * u_stretch + u_translation[0]) / u_aspectRatio, position[1]  * u_stretch + u_translation[1], 0, 1);\n"
		"	gl_Position[2] = 0;\n"
		"   v_uvCoord = uvCoord;\n"
		"   v_color = color;\n"
		"   v_tex = tex;\n"
		"};\n";

	std::string fsString = 
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"in vec2 v_uvCoord;\n"
		"in vec4 v_color;\n"
		"in float v_tex;\n"
		"\n"
		"uniform sampler2D u_textures[15];\n"
		"\n"
		"uniform vec4 u_textColor;\n"
		"uniform vec4 u_textBGColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	int t = int(v_tex);\n"
		"	color = v_color;\n"
		"   if (t != -1)\n"
		"      color = texture(u_textures[t], v_uvCoord); \n"
		"   if ((color[0] + color[1] + color[2]) / 3 < 0.5f)\n"
		"      color = u_textBGColor;\n"
		"   else\n"
		"      color = u_textColor;\n"
		"};\n";
}

Shader BitmapText::textShader = Shader(vsString, fsString);

BitmapText::BitmapText(BitmapFont* font) :
	font(font)
{
}

BitmapText::BitmapText() :
	font(nullptr)
{
}


void BitmapText::setText(const std::string & text)
{
	this->text = text;
	renderCount = text.length();
	textHasChanged = true;
}

const std::string & BitmapText::getText()
{
	return text;
}

void BitmapText::setColor(float r, float g, float b, float a)
{
	color = { r, g, b, a };
}

void BitmapText::setColor(const std::array<float, 4>& color)
{
	this->color = color;
}

float BitmapText::getR()
{
	return color[0];
}

float BitmapText::getG()
{
	return color[1];
}

float BitmapText::getB()
{
	return color[2];
}

void BitmapText::setBackgroundColor(float r, float g, float b, float a)
{
	bgColor = { r, g, b, a };
}

void BitmapText::setBackgroundColor(const std::array<float, 4>& color)
{
	bgColor = color;
}

float BitmapText::getBackgroundR()
{
	return bgColor[0];
}

float BitmapText::getBackgroundG()
{
	return bgColor[1];
}

float BitmapText::getBackgroundB()
{
	return bgColor[2];
}

float BitmapText::getBackgroundA() {
	return bgColor[3];
}

void BitmapText::setCharacterRenderCount(int x)
{
	renderCount = x;
	textHasChanged = true;
}

int BitmapText::getCharacterRenderCount()
{
	return renderCount;
}

void BitmapText::resetRenderCount()
{
	renderCount = text.length();
	textHasChanged = true;
}

void BitmapText::setPosition(const std::array<float, 2>& pos)
{
	position = pos;
}

void BitmapText::setPosition(float x, float y)
{
	position = { x, y };
}

float BitmapText::getX()
{
	return position[0];
}

float BitmapText::getY()
{
	return position[1];
}

void BitmapText::setCharHeight(float height)
{
	charHeight = height;
}

float BitmapText::getCharHeight()
{
	return charHeight;
}

void BitmapText::setFont(BitmapFont * font)
{
	this->font = font;
}

BitmapFont* BitmapText::getFont()
{
	return font;
}

void BitmapText::render()
{
	if (!font) {
		font = FontManager::getDefaultFont();
	}

	bq.useShader(textShader);

	if (textHasChanged) {
		bq.setTextureSlot(0, &font->getTexture());
		bq.clear();

		float x = 0, y = 0;
		float width = charHeight, height = charHeight;
		for (int i = 0; i < text.length() && i < renderCount; i++) {

			width = (font->getCharacterInfo(text[i]).widthSample / font->getCharacterInfo(text[i]).heightSample) * charHeight;

			bq.addQuad(x, y, width, height, color[0], color[1], color[2], color[3], 0);
			bq.setQuadTextureSampleBounds(i, font->getSampleArea(text[i])[0], font->getSampleArea(text[i])[1], font->getSampleArea(text[i])[2], font->getSampleArea(text[i])[3]);

			x += width;
		}

		textHasChanged = false;
	}

	if (bq.size() > 0) {
		textShader.setUniform4f("u_textColor", color[0], color[1], color[2], color[3]);
		textShader.setUniform4f("u_textBGColor", bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
		bq.renderAll(1.0f, position);
	}
}
