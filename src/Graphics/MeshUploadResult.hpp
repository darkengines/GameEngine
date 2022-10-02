#pragma once

#include "../Devices/Buffer.hpp"
#include "../Meshes/Mesh.hpp"

namespace drk::Graphics {
	struct MeshUploadResult {
		Devices::Buffer indexBuffer;
		Devices::Buffer vertexBuffer;
		std::vector<Meshes::Mesh> meshes;
	};
}
