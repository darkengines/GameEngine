#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <optional>

namespace drk::Textures {
	struct ImageInfo;
}
namespace drk::Materials {
	struct Material {
		std::string name;

		glm::vec4 baseColor;
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;

		std::optional<Textures::ImageInfo *> baseColorTexture;
		std::optional<Textures::ImageInfo *> ambientColorTexture;
		std::optional<Textures::ImageInfo *> diffuseColorTexture;
		std::optional<Textures::ImageInfo *> specularColorTexture;
		std::optional<Textures::ImageInfo *> normalMap;
		std::optional<Textures::ImageInfo *> metallicRoughnessTexture;

		bool hasAlpha;
	};
}
