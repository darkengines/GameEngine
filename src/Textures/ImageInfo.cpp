#include <stb_image.h>
#include "ImageInfo.hpp"
#include <memory>
#include <cmath>
#include <string>

namespace drk::Textures {
	ImageInfo ImageInfo::fromFile(const std::string& name, const std::string& filePath, TextureType type) {
		ImageInfo imageInfo{
			type,
			0u,
			0u,
			0u,
			std::vector<unsigned char>{}
		};
		auto pixels = stbi_load(
			filePath.c_str(),
			(int*) &imageInfo.width,
			(int*) &imageInfo.height,
			(int*) &imageInfo.depth,
			4u
		);
		imageInfo.pixels.assign(pixels, pixels + imageInfo.width * imageInfo.height * 4u * sizeof(unsigned char));
		free(pixels);
		return imageInfo;
	}

	ImageInfo ImageInfo::fromMemory(
		const std::string& name,
		const std::span<unsigned char>& memory,
		TextureType type
	) {
		ImageInfo imageInfo{
			type,
			0u,
			0u,
			0u,
			std::vector<unsigned char>{}
		};
		auto pixels = stbi_load_from_memory(
			memory.data(),
			(int) memory.size(),
			(int*) &imageInfo.width,
			(int*) &imageInfo.height,
			(int*) &imageInfo.depth,
			4
		);
		imageInfo.pixels.assign(pixels, pixels + imageInfo.width * imageInfo.height * 4u * sizeof(unsigned char));
		free(pixels);
		return imageInfo;
	}

	std::unordered_map<TextureType, vk::Format> ImageInfo::TextureTypeFormatMap{
		{TextureType::AlbedoColor,           vk::Format::eR8G8B8A8Srgb},
		{TextureType::AmbientColor,          vk::Format::eR8G8B8A8Srgb},
		{TextureType::AmbientOcclusionMap,   vk::Format::eR8G8B8A8Unorm},
		{TextureType::BaseColor,             vk::Format::eR8G8B8A8Srgb},
		{TextureType::DiffuseColor,          vk::Format::eR8G8B8A8Srgb},
		{TextureType::DiffuseRoughnessMap,   vk::Format::eR8G8B8A8Unorm},
		{TextureType::DisplacementMap,       vk::Format::eR8G8B8A8Unorm},
		{TextureType::EmissionColor,         vk::Format::eR8G8B8A8Srgb},
		{TextureType::EmissionMap,           vk::Format::eR8G8B8A8Unorm},
		{TextureType::HeightMap,             vk::Format::eR8G8B8A8Unorm},
		{TextureType::LightMap,              vk::Format::eR8G8B8A8Unorm},
		{TextureType::MetalnessMap,          vk::Format::eR8G8B8A8Unorm},
		{TextureType::None,                  vk::Format::eUndefined},
		{TextureType::NormalCameraMap,       vk::Format::eR8G8B8A8Unorm},
		{TextureType::NormalMap,             vk::Format::eR8G8B8A8Unorm},
		{TextureType::OpacityMap,            vk::Format::eR8G8B8A8Unorm},
		{TextureType::ReflectionMap,         vk::Format::eR8G8B8A8Unorm},
		{TextureType::RoughnessMap,          vk::Format::eR8G8B8A8Unorm},
		{TextureType::RoughnessMetalnessMap, vk::Format::eR8G8B8A8Unorm},
		{TextureType::ShininessMap,          vk::Format::eR8G8B8A8Unorm},
		{TextureType::SpecularColor,         vk::Format::eR8G8B8A8Srgb},
	};

	uint32_t ImageInfo::mipLevels() const {
		return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height))));
	}
}
