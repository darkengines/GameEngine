#pragma once

#include <cstdint>
#include "TextureType.hpp"
#include <memory>
#include <string>
#include <vector>
#include <span>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace drk::Textures {
	struct ImageInfo {
		TextureType type;
		uint32_t width;
		uint32_t height;
		uint32_t depth;
		std::vector<unsigned char> pixels;

		static std::unique_ptr<ImageInfo>
		fromFile(const std::string& name, const std::string& filePath, TextureType type);

		static std::unique_ptr<ImageInfo>
		fromMemory(const std::string& name, const std::span<unsigned char>& memory, TextureType type);

		static std::unordered_map<TextureType, vk::Format> TextureTypeFormatMap;
		[[nodiscard]] uint32_t mipLevels() const;
	};
}