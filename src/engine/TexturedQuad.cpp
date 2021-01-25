#include "engine/TexturedQuad.h"
#include "engine/Window.h"

#include "engine/ViewportManager.h"

TexturedQuad::TexturedQuad(float x, float y, float width, float height, Texture & texture) :
	tex(&texture),
	x(x),
	y(y),
	width(width),
	height(height)
{
}

TexturedQuad::TexturedQuad()
{
	tex = nullptr;
	x = 0;
	y = 0;
	width = 1;
	height = 1;
}

void TexturedQuad::setX(float x) {
	this->x = x;
}

void TexturedQuad::setY(float y) {
	this->y = y;
}

void TexturedQuad::setWidth(float width) {
	this->width = width;
}

void TexturedQuad::setHeight(float height) {
	this->height = height;
}


void TexturedQuad::setBounding(float x, float y, float width, float height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void TexturedQuad::setTexture(Texture & texture) {
	this->tex = &texture;
}

float TexturedQuad::getX()
{
	return x;
}

float TexturedQuad::getY()
{
	return y;
}

float TexturedQuad::getWidth()
{
	return width;
}

float TexturedQuad::getHeight()
{
	return height;
}

Texture* TexturedQuad::getTexture()
{
	return tex;
}

void TexturedQuad::setTextureSampleArea(float x, float y, float width, float height) {
	texX = x;
	texY = y;
	texWidth = width;
	texHeight = height;
}

void TexturedQuad::setTextureSampleArea(int pixX, int pixY, float pixWidth, float pixHeight)
{
	texX = (float)pixX / tex->getWidth();
	texY = (float)pixY / tex->getHeight();
	texWidth = (float)pixWidth / tex->getWidth();
	texHeight = (float)pixHeight / tex->getHeight();
}

void TexturedQuad::setShader(Shader& shader)
{
	this->shader = &shader;
}

void TexturedQuad::render() {
	static float vertices[] = {
		0.0f, 0.0f,  0.0f, 0.0f,
		0.0f, 1.0f,  0.0f, 1.0f,
		1.0f, 1.0f,  1.0f, 1.0f,
		1.0f, 0.0f,  1.0f, 0.0f,
	};
	static VertexBuffer vb = VertexBuffer(vertices, 4 * sizeof(float) * 4);

	static unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	static IndexBuffer ib = IndexBuffer(indices, 6 * sizeof(unsigned int));

	static VertexArray va = VertexArray("ff ff", vb, ib);

	static std::string vertexShaderString = 
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec2 position;\n"
		"layout(location = 1) in vec2 uvCoord;\n"
		"\n"
		"uniform vec2 u_stretch;\n"
		"uniform vec2 u_translation;\n"
		"uniform float u_aspectRatio;\n"
		"\n"
		"uniform vec2 u_texture_stretch;\n"
		"uniform vec2 u_texture_translation;\n"
		"\n"
		"out vec2 v_texCoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4((position[0] / u_aspectRatio) * u_stretch[0] + (u_translation[0] / u_aspectRatio), position[1] * u_stretch[1] + u_translation[1], 0, 1);\n"
		"	v_texCoord = vec2(uvCoord[0] * u_texture_stretch[0] + u_texture_translation[0], uvCoord[1] * u_texture_stretch[1] + u_texture_translation[1]);\n"
		"};\n";

	static std::string fragmentShaderString =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"in vec2 v_texCoord;\n"
		"\n"
		"uniform sampler2D u_texture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = texture(u_texture, v_texCoord);\n"
		"};\n";

	static Shader sh = Shader(vertexShaderString, fragmentShaderString);

	if (!shader) {
		sh.setUniform1f("u_aspectRatio", ViewportManager::getCurrentAspectRatio());
		sh.setUniform1i("u_texture", 0);
		sh.setUniform2f("u_stretch", width, height);
		sh.setUniform2f("u_translation", x, y);
		sh.setUniform2f("u_texture_stretch", texWidth, texHeight);
		sh.setUniform2f("u_texture_translation", texX, texY);
	}
	else {
		shader->setUniform1f("u_aspectRatio", ViewportManager::getCurrentAspectRatio());
		shader->setUniform1i("u_texture", 0);
		shader->setUniform2f("u_stretch", width, height);
		shader->setUniform2f("u_translation", x, y);
		shader->setUniform2f("u_texture_stretch", texWidth, texHeight);
		shader->setUniform2f("u_texture_translation", texX, texY);
	}

	if (tex == nullptr) {
		static Texture defaultTexture(512, 512);
		defaultTexture.bind(0);
	}
	else {
		tex->bind();
	}

	if (!shader) {
		sh.bind();
		va.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
		va.unbind();
		sh.unbind();
		tex->unbind();
	}
	else {
		shader->bind();
		va.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
		va.unbind();
		shader->unbind();
		tex->unbind();
	}
}
