#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "FrustumVertex.hpp"

namespace drk::Frustums::Models {
	struct Vertex {
		FrustumVertex position;
		glm::vec4 diffuseColor;

		static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();
		static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();
	};
}