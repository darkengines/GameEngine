#pragma once

#include "../Stores/Models/StoreItemLocation.hpp"
#include "../Meshes/Components/Mesh.hpp"
#include "../Meshes/MeshInfo.hpp"
#include "../Spatials/Components/Spatial.hpp"

namespace drk::Graphics {
	struct Draw {
		Meshes::MeshInfo* meshInfo;
		Meshes::Components::Mesh mesh;
		Stores::Models::StoreItemLocation meshStoreItem;
		Stores::Models::StoreItemLocation objectLocation;
		Spatials::Components::Spatial spatial;
		bool hasTransparency;
	};
}