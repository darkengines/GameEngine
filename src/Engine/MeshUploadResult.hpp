#pragma once

#include "../Devices/Buffer.hpp"
#include "../Meshes/Mesh.hpp"

namespace drk::Engine {
	struct MeshUploadResult {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector<Meshes::Mesh> meshes;
	};
}
