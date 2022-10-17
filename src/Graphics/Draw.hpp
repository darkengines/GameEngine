#pragma once

#include "Models/StoreItemLocation.hpp"
#include "../Meshes/Mesh.hpp"
#include "../Meshes/MeshInfo.hpp"

namespace drk::Graphics {
	struct Draw {
		Meshes::MeshInfo * meshInfo;
		Meshes::Mesh mesh;
		Models::StoreItemLocation meshStoreItem;
		Models::StoreItemLocation objectLocation;
	};
}