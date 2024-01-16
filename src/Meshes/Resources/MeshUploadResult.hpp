#pragma once

#include <vector>
#include "../../Devices/Buffer.hpp"
#include "../../Meshes/Components/MeshBufferView.hpp"

namespace drk::Meshes::Resources {
	struct MeshUploadResult {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector<Meshes::Components::MeshBufferView> meshes;
	};
}
