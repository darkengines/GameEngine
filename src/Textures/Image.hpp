#pragma once

#include "TextureType.hpp"
#include <memory>
#include <string>
#include <vector>
#include <span>

namespace drk::Textures {
	struct Image {
		std::string name;
		TextureType type;
		uint32_t width;
		uint32_t height;
		uint32_t depth;

		std::vector<unsigned char> pixels;

		static std::unique_ptr<Image>
		fromFile(const std::string &name, const std::string &filePath, TextureType type);

		static std::unique_ptr<Image>
		fromMemory(const std::string &name, const std::span<unsigned char> memory, TextureType type);
	};
}