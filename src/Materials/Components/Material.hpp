#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <optional>
#include <entt/entt.hpp>

namespace drk::Materials::Components {
	struct Material {
		std::string source;

		glm::vec4 baseColor;
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;

		entt::entity baseColorTexture = entt::null;
		entt::entity ambientColorTexture = entt::null;
		entt::entity diffuseColorTexture = entt::null;
		entt::entity specularColorTexture = entt::null;
		entt::entity normalMap = entt::null;
		entt::entity metallicRoughnessTexture = entt::null;
		entt::entity metallicTexture = entt::null;
		entt::entity roughnessTexture = entt::null;

		bool hasTransparency;
	};
}
