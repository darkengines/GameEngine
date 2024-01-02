#include "Vertex.hpp"

namespace drk::BoundingVolumes::Models {

	std::vector<vk::VertexInputBindingDescription> Vertex::getBindingDescriptions() {
		vk::VertexInputBindingDescription bindingDescription = {
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex
		};

		return { bindingDescription };
	}

	std::vector<vk::VertexInputAttributeDescription> Vertex::getAttributeDescriptions() {

		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, diffuseColor);

		return attributeDescriptions;
	}
}