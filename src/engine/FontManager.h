#pragma once

#include "engine/BitmapFont.h"
#include <map>
#include <string>

class FontManager {
public:
	static void init();
	static BitmapFont* getDefaultFont();
	static BitmapFont* getFont(const std::string& filepath);
	static void cleanup();

private:
	static std::map<std::string, BitmapFont*> fonts;

};