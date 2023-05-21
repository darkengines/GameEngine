#pragma once

#include "../Devices/Buffer.hpp"
#include "../Meshes/Components/MeshBufferView.hpp"

namespace drk::Engine {
	struct MeshUploadResult {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector<Meshes::Components::MeshBufferView> meshes;
	};
}
