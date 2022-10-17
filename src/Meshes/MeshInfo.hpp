#pragma once

#include <string>
#include <vector>
#include <entt/entity/entity.hpp>
#include "Vertex.hpp"

namespace drk::Materials {
	struct Material;
}
namespace drk::Meshes {
	struct MeshInfo {
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<VertexIndex> indices;
		Materials::Material* pMaterial;
		entt::entity materialEntity;
	};
}