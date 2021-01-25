#pragma once

#include "engine/Texture.h"
#include <map>

class TextureManager {
public:
	static Texture* getTexture(const std::string& filepath);
	static void cleanup();

private:
	static std::map<std::string, Texture*> textures;

};