#include <stb_image.h>
#include "Image.hpp"

namespace drk::Textures {
	std::unique_ptr<Image>
	Image::fromFile(const std::string &name, const std::string &filePath, TextureType type) {
		auto texture = std::make_unique<Image>(
			name,
			type,
			0u,
			0u,
			0u,
			std::vector<unsigned char>{}
		);
		auto pixels = stbi_load(
			texture->name.c_str(),
			(int *) &texture->width,
			(int *) &texture->height,
			(int *) &texture->depth,
			4u
		);
		texture->pixels.assign(pixels, pixels + texture->width * texture->height * 4u * sizeof(unsigned char));
		free(pixels);
		return texture;
	}

	std::unique_ptr<Image>
	Image::fromMemory(
		const std::string &name,
		const std::span<unsigned char> memory,
		TextureType type
	) {
		auto texture = std::make_unique<Image>(
			name,
			type,
			0u,
			0u,
			0u,
			std::vector<unsigned char>{}
		);
		auto pixels = stbi_load_from_memory(
			memory.data(),
			memory.size(),
			(int *) &texture->width,
			(int *) &texture->height,
			(int *) &texture->depth,
			4
		);
		texture->pixels.assign(pixels, pixels + texture->width * texture->height * 4u * sizeof(unsigned char));
		free(pixels);
		return texture;
	}
}
