#pragma once

#include <glm/vec4.hpp>

namespace drk::Materials::Models {
	struct Material {
		glm::vec4 baseColor;
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;

		uint32_t hasBaseColorTexture;
		uint32_t hasAmbientColorTexture;
		uint32_t hasDiffuseColorTexture;
		uint32_t hasSpecularColorTexture;
		uint32_t hasNormalMap;
		uint32_t hasMetallicRoughnessMap;

		uint32_t baseColorTextureIndex;
		uint32_t ambientColorTextureIndex;
		uint32_t diffuseColorTextureIndex;
		uint32_t specularColorTextureIndex;
		uint32_t normalMapIndex;
		uint32_t metallicRoughnessTextureIndex;
	};
}
