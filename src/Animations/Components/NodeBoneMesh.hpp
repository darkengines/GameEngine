#pragma once

#include <entt/entt.hpp>

namespace drk::Animations::Components {
	struct NodeBoneNodeMesh {
		entt::entity nodeBoneEntity;
		entt::entity rootNodeBoneEntity;
		entt::entity nodeMeshEntity;
		entt::entity boneMeshEntity;
	};
}
