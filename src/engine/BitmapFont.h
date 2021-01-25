#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>

#include "engine/Texture.h"

struct BitmapFontCharacterInfo {
	float xSample;
	float ySample;
	float widthSample;
	float heightSample;

	float xOffset;
	float yOffset;
	float xAdvance;
};

class BitmapFont {
public:
	BitmapFont(const std::string& filepath);

	const std::string& getFilepath();

	Texture& getTexture();
	std::array<float, 4> getSampleArea(char c);
	const BitmapFontCharacterInfo& getCharacterInfo(char c);

private:
	const std::string filepath;

	std::array<BitmapFontCharacterInfo, 256> loadedSampleCoords;
	Texture loadedTextures;

	bool hasInitialized = false;
	void tryInitialize();
};


