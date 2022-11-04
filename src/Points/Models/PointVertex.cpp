#include "PointVertex.hpp"

namespace drk::Points::Models {

	std::vector<vk::VertexInputBindingDescription> PointVertex::getBindingDescriptions() {
		vk::VertexInputBindingDescription bindingDescription = {
			.binding = 0,
			.stride = sizeof(PointVertex),
			.inputRate = vk::VertexInputRate::eVertex
		};

		return {bindingDescription};
	}

	std::vector<vk::VertexInputAttributeDescription> PointVertex::getAttributeDescriptions() {

		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(3);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[0].offset = offsetof(PointVertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[1].offset = offsetof(PointVertex, diffuseColor);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[2].offset = offsetof(PointVertex, textureCoordinates);


		return attributeDescriptions;
	}
}