#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drk::BoundingVolumes::Models {
	struct Vertex {
		glm::vec4 position;
		glm::vec4 diffuseColor;

		static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
	};
}