#pragma once
#include <stdint.h>
#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct VertexWeight {
		entt::entity boneEntity;
		entt::entity meshEntity;
		uint32_t vertexIndex;
		float weight;
	};
}