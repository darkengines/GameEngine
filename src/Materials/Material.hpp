#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <optional>

namespace drk::Textures {
	struct Image;
}
namespace drk::Materials {
	struct Material {
		std::string name;

		glm::vec4 baseColor;
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;

		std::optional<Textures::Image *> baseColorTexture;
		std::optional<Textures::Image *> ambientColorTexture;
		std::optional<Textures::Image *> diffuseColorTexture;
		std::optional<Textures::Image *> specularColorTexture;
		std::optional<Textures::Image *> normalMap;
		std::optional<Textures::Image *> metallicRoughnessTexture;

		bool hasAlpha;
	};
}
