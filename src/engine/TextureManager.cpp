#include "TextureManager.h"

std::map<std::string, Texture*> TextureManager::textures;

Texture * TextureManager::getTexture(const std::string & filepath)
{
	if (textures.find(filepath) == textures.end()) {
		textures[filepath] = new Texture(filepath);
	}

	return textures[filepath];
}

void TextureManager::cleanup()
{
	for (auto& element : textures) {
		delete element.second;
		element.second = nullptr;
	}
}
