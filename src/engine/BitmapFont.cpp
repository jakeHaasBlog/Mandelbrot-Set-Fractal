#include "BitmapFont.h"

#include <fstream>


BitmapFont::BitmapFont(const std::string & filepath) :
	filepath(filepath)
{
}


const std::string & BitmapFont::getFilepath()
{
	return filepath;
}

Texture & BitmapFont::getTexture()
{
	tryInitialize();
	return loadedTextures;
}

std::array<float, 4> BitmapFont::getSampleArea(char c)
{
	tryInitialize();
	return { loadedSampleCoords[(int)c].xSample, loadedSampleCoords[(int)c].ySample, loadedSampleCoords[(int)c].widthSample, loadedSampleCoords[(int)c].heightSample };
}

const BitmapFontCharacterInfo & BitmapFont::getCharacterInfo(char c)
{
	tryInitialize();
	return loadedSampleCoords[(int)c];
}

void BitmapFont::tryInitialize()
{
	if (!hasInitialized) {

		loadedTextures.generateFromFile(filepath + "/texture.png");

		loadedSampleCoords = std::array<BitmapFontCharacterInfo, 256>();

		std::ifstream stream(filepath + "/info.txt");

		int charID;
		int x, y, width, height;
		float xOffset, yOffset, xAdvance;
		std::string c;
		while (stream >> charID >> x >> y >> width >> height >> xOffset >> yOffset >> xAdvance) {

			y = loadedTextures.getHeight() - y;
			y -= height;

			float screenCoordsX = (float)x / loadedTextures.getWidth();
			float screenCoordsY = (float)y / loadedTextures.getHeight();
			float normWidth = (float)width / loadedTextures.getWidth();
			float normHeight = (float)height / loadedTextures.getHeight();


			if (!(charID >= 256 || charID < 0))
				loadedSampleCoords[charID] = { screenCoordsX, screenCoordsY, normWidth, normHeight, xOffset, yOffset, xAdvance };

		}

		stream.close();


		hasInitialized = true;
	}
}
