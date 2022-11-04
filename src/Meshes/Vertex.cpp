#include "Vertex.hpp"

namespace drk::Meshes {
	std::vector<vk::VertexInputBindingDescription> Vertex::getBindingDescriptions() {
		vk::VertexInputBindingDescription bindingDescription = {
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex
		};

		return {bindingDescription};
	}

	std::vector<vk::VertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(6);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[2].offset = offsetof(Vertex, tangent);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[3].offset = offsetof(Vertex, bitangent);

		attributeDescriptions[4].binding = 0;
		attributeDescriptions[4].location = 4;
		attributeDescriptions[4].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[4].offset = offsetof(Vertex, diffuseColor);

		attributeDescriptions[5].binding = 0;
		attributeDescriptions[5].location = 5;
		attributeDescriptions[5].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[5].offset = offsetof(Vertex, textureCoordinates);


		return attributeDescriptions;
	}
}