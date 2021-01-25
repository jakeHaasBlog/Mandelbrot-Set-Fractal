#include "engine/Texture.h"

#include <array>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>

#include "engine/Window.h"
#include <iostream>

#include "ViewportManager.h"

std::stack<GLuint> Texture::frameBufferIDhistory;

Texture::Texture() {
	isInitilized = false;
	initType = 0;
}

Texture::Texture(GLuint id)
{
	this->id = id;
	isInitilized = true;
	initType = 1;
}

Texture::Texture(int width, int height) {
	//generateDefaultTexture(width, height);
	initType = 2;
	initPixWide = width;
	initPixHigh = height;
}

Texture::Texture(int width, int height, float * data, size_t pixelCount) {
	//generateFromData(width, height, data, pixelCount);
	initType = 3;
	initPixWide = width;
	initPixHigh = height;
	initData = new float[pixelCount * 4];
	memcpy_s(initData, sizeof(float) * pixelCount * 4, data, pixelCount * sizeof(float) * 4);
}

Texture::Texture(const std::string & filename) {
	//generateFromFile(filename);
	initType = 4;
	initFilepath = filename;
}

Texture::~Texture()
{
	if (isInitilized) {
		freeMemory();
	}
}

void Texture::generateFromFile(const std::string & filename) {
	initType = 4;

	stbi_set_flip_vertically_on_load(true);

	int bitsPerPixel;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &bitsPerPixel, 4);
	std::vector<std::array<float, 4>> convertedPixels(width * height);

	for (int i = 0; i < width * height * 4; i++) {
		int pixIndex = i / 4;
		int pixPart = i % 4;

		convertedPixels[pixIndex][pixPart] = (float)data[i] / 255;
	}

	delete[] data;

	if (!isInitilized) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_2D, id);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, &convertedPixels[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	isInitilized = true;
}

void Texture::generateDefaultTexture(int width, int height) {
	initType = 2;

	this->width = width;
	this->height = height;
	if (!isInitilized) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_2D, id);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, id);
	
	std::vector<std::array<float, 4>> pixels(width * height);
	float r, g, b;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
	
			pixels[x + y * width][0] = 0.0f;
			pixels[x + y * width][1] = 1.0f;
			pixels[x + y * width][2] = 0.2f;
			pixels[x + y * width][3] = 1.0f;
	
			if ((x / 50) % 2 == 0 != (y / 50) % 2 == 1) {
				pixels[x + y * width][0] = 1.0f;
				pixels[x + y * width][1] = 0.0f;
				pixels[x + y * width][2] = 1.0f;
				pixels[x + y * width][3] = 1.0f;
			}
	
		}
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
	
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	isInitilized = true;
}

void Texture::generateFromData(int width, int height, float * data, size_t pixelCount) {
	initType = 3;

	this->width = width;
	this->height = height;
	if (!isInitilized) {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	isInitilized = true;
}

void Texture::setDefaultTexParameters() {
	tryInitialize();

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int textureSlot) {
	tryInitialize();
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bindAsRenderTarget()
{
	tryInitialize();

	if (!frameBufferInitilaized) {
		generateFrameBuffer();
		frameBufferInitilaized = true;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	ViewportManager::bindViewportPixels(0, 0, width, height);

	frameBufferIDhistory.push(frameBufferID);
}

void Texture::unbindAsRenderTarget()
{
		ViewportManager::unbindViewport();

		glBindTexture(GL_TEXTURE_2D, id);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		frameBufferIDhistory.pop();
		if (frameBufferIDhistory.size() > 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferIDhistory.top());
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
}

void Texture::setSamplingMode(int sampleMode) {
	tryInitialize();
	if (sampleMode == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (sampleMode == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

void Texture::setTextureWrap(int wrappingMethod) {
	tryInitialize();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMethod);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMethod);
}

std::vector<std::array<float, 4>> Texture::getPixels(int mipmapLevel) {
	tryInitialize();
	std::vector<std::array<float, 4>> pixels(width * height);

	glBindTexture(GL_TEXTURE_2D, id);
	glGetTexImage(GL_TEXTURE_2D, mipmapLevel, GL_RGBA, GL_FLOAT, &pixels[0]);
	unbind();

	return pixels;
}

void Texture::saveToFile(const std::string& filepath)
{

	std::vector<std::array<float, 4>> pixels(width * height);
	glBindTexture(GL_TEXTURE_2D, id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &pixels[0]);
	unbind();


	uint8_t* pixels_int = new uint8_t[width * height * 4];
	int i = 0;
	for (std::array<float, 4>&p : pixels) {
		p[0] = std::clamp(p[0], 0.0f, 1.0f);
		p[1] = std::clamp(p[1], 0.0f, 1.0f);
		p[2] = std::clamp(p[2], 0.0f, 1.0f);
		p[3] = std::clamp(p[3], 0.0f, 1.0f);

		uint8_t r = (uint8_t)(p[0] * 254.0f);
		uint8_t g = (uint8_t)(p[1] * 254.0f);
		uint8_t b = (uint8_t)(p[2] * 254.0f);
		uint8_t a = (uint8_t)(p[3] * 254.0f);

		pixels_int[i + 0] = r;
		pixels_int[i + 1] = g;
		pixels_int[i + 2] = b;
		pixels_int[i + 3] = a;

		i += 4;
	}

	stbi_flip_vertically_on_write(true);
	stbi_write_png(filepath.c_str(), width, height, 4, pixels_int, 4 * width);
	delete[] pixels_int;

}

GLuint Texture::getID() {
	tryInitialize();
	return id;
}

int Texture::getWidth()
{
	tryInitialize();
	return width;
}

int Texture::getHeight()
{
	tryInitialize();
	return height;
}

void Texture::freeMemory() {
	if (isInitilized) {
		if (frameBufferInitilaized) {
			glDeleteFramebuffers(1, &frameBufferID);
		}
		glDeleteTextures(1, &id);
	}

	if (initData) {
		delete[] initData;
		initData = nullptr;
	}

	unbind();
}

void Texture::generateFrameBuffer()
{

	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Texture::tryInitialize()
{
	if (!isInitilized) {
		switch (initType) {
		case 0: // none
			__debugbreak(); // if you get an error here then you did not call a generate function before using this texture
			break;
		case 1: // byID
			break;
		case 2: // default
			generateDefaultTexture(initPixWide, initPixHigh);
			break;
		case 3: // byData
			generateFromData(initPixWide, initPixHigh, initData, initPixCount);
			delete[] initData;
			initData = nullptr;
			break;
		case 4: // byFile
			generateFromFile(initFilepath);
			break;
		}
		isInitilized = true;
	} 
}
