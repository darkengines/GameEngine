#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace drk::Lines::Models {
	struct LineVertex { ;
		glm::vec4 position;
		glm::vec4 diffuseColor;
		glm::vec2 textureCoordinates;

		static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
	};
}