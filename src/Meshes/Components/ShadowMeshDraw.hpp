
#pragma once

#include "MeshResource.hpp"
#include "MeshBufferView.hpp"
#include "../../Stores/StoreItemLocation.hpp"

namespace drk::Meshes::Components {
	struct ShadowMeshDraw {
		std::shared_ptr<MeshResource> meshResource;
		MeshBufferView meshBufferView;
		entt::entity meshEntity;
		entt::entity objectEntity;
		entt::entity cameraEntity;
		entt::entity lightPerspectiveEntity;
		entt::entity lightPerspectiveSpatialEntity;
	};
}