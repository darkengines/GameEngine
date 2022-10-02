#pragma once

#include "../system.hpp"
#include <glm/glm.hpp>
#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace drk::Meshes {
	typedef uint32_t VertexIndex;
	struct Vertex {
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec4 tangent;
		glm::vec4 bitangent;
		glm::vec4 diffuseColor;
		glm::vec2 textureCoordinates;

		static std::vector<vk::VertexInputBindingDescription> getBindingDescription();
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
	};
}