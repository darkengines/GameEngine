#pragma once

#include "../Stores/Models/StoreItemLocation.hpp"
#include "../Meshes/Mesh.hpp"
#include "../Meshes/MeshInfo.hpp"
#include "../Spatials/Spatial.hpp"

namespace drk::Graphics {
	struct Draw {
		Meshes::MeshInfo* meshInfo;
		Meshes::Mesh mesh;
		Stores::Models::StoreItemLocation meshStoreItem;
		Stores::Models::StoreItemLocation objectLocation;
		Spatials::Spatial spatial;
		bool hasTransparency;
	};
}