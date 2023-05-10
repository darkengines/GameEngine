#pragma once

#include "../Devices/Buffer.hpp"
#include "../Meshes/Components/Mesh.hpp"

namespace drk::Engine {
	struct MeshUploadResult {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector<Meshes::Components::Mesh> meshes;
	};
}
