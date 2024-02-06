#pragma once
#include <entt/entt.hpp>
namespace drk::Animations::Components {
	struct VertexWeightInstance {
		uint32_t vertexIndex;
		float weight;
		entt::entity boneInstanceEntity;
		entt::entity boneEntity;
	};
}