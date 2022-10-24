#pragma once

#include <string>
#include <vector>
#include <entt/entity/entity.hpp>
#include "Vertex.hpp"
#include "../Materials/Material.hpp"

namespace drk::Meshes {
	struct MeshInfo {
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<VertexIndex> indices;
		Materials::Material* pMaterial;
		entt::entity materialEntity;
	};
}