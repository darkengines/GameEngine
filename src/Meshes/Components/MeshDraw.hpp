
#pragma once

#include "MeshResource.hpp"
#include "MeshBufferView.hpp"
#include "../../Stores/StoreItemLocation.hpp"

namespace drk::Meshes::Components {
	struct MeshDraw {
		uint32_t indexCount;
		MeshBufferView meshBufferView;
	};
}