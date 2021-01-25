#include "FontManager.h"

std::map<std::string, BitmapFont*> FontManager::fonts;

void FontManager::init()
{
	fonts["assets/fonts/defaultFont"] = new BitmapFont("assets/fonts/defaultFont");
}

BitmapFont * FontManager::getDefaultFont()
{
	return fonts["assets/fonts/defaultFont"];
}

BitmapFont * FontManager::getFont(const std::string & filepath)
{
	if (fonts.find(filepath) == fonts.end()) {
		fonts[filepath] = new BitmapFont(filepath);
	}

	return fonts[filepath];
}

void FontManager::cleanup()
{
	for (auto it = fonts.begin(); it != fonts.end(); it++) {
		delete it->second;
		it->second = nullptr;
	}
}
