
#pragma once

#include "MeshResource.hpp"
#include "MeshBufferView.hpp"
#include "../../Stores/StoreItemLocation.hpp"

namespace drk::Meshes::Components {
	struct ShadowMeshDraw {
		uint32_t indexCount;
		MeshBufferView meshBufferView;
		entt::entity meshEntity;
		entt::entity nodeEntity;
		entt::entity cameraEntity;
		entt::entity lightPerspectiveEntity;
		entt::entity lightPerspectiveSpatialEntity;
	};
}