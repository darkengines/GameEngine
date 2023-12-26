
#pragma once

#include "MeshResource.hpp"
#include "MeshBufferView.hpp"
#include "../../Stores/StoreItemLocation.hpp"

namespace drk::Meshes::Components {
	struct ShadowMeshDraw {
		std::shared_ptr<MeshResource> meshResource;
		MeshBufferView meshBufferView;
		alignas(8) Stores::Models::StoreItemLocation meshItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
		alignas(8) Stores::Models::StoreItemLocation cameraItemLocation;
		alignas(8) Stores::Models::StoreItemLocation lightPerspectiveItemLocation;
		alignas(8) Stores::Models::StoreItemLocation lightPerspectiveSpatialItemLocation;
	};
}