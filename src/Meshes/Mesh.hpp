#pragma once

#include <string>
#include <vector>
#include "Vertex.hpp"

namespace drk::Materials {
	struct Material;
}
namespace drk::Meshes {
	struct Mesh {
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Materials::Material *material;
	};
}