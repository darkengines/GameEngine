#pragma once

#include "Models/StoreItemLocation.hpp"
#include "../Meshes/Mesh.hpp"
#include "../Meshes/MeshInfo.hpp"
#include "../Spatials/Spatial.hpp"

namespace drk::Graphics {
	struct Draw {
		Meshes::MeshInfo * meshInfo;
		Meshes::Mesh mesh;
		Models::StoreItemLocation meshStoreItem;
		Models::StoreItemLocation objectLocation;
		Spatials::Spatial spatial;
		bool hasTransparency;
	};
}