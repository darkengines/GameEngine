
#pragma once

#include "../MeshInfo.hpp"
#include "Mesh.hpp"
#include "../../Stores/StoreItemLocation.hpp"

namespace drk::Meshes::Components {
	struct MeshDraw {
		MeshInfo* meshInfo;
		Mesh mesh;
		alignas(8) Stores::Models::StoreItemLocation meshItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
	};
}