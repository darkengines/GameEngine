#pragma once

#include <entt/entt.hpp>
#include "../../Spatials/Components/Spatial.hpp"
#include "VertexWeight.hpp"

namespace drk::Animations::Components {
	struct BoneMesh {
		entt::entity boneEntity;
		entt::entity skinnedMeshEntity;
		Spatials::Components::Spatial<Spatials::Components::Relative> offset;
		std::vector<Animations::Components::VertexWeight> vertexWeights;
	};
}