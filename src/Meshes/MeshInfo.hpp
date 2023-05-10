#pragma once

#include <string>
#include <vector>
#include <entt/entity/entity.hpp>
#include "Vertex.hpp"
#include "../Materials/Components/Material.hpp"

namespace drk::Meshes {
	struct MeshInfo {
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<VertexIndex> indices;
		Materials::Components::Material* pMaterial;
		entt::entity materialEntity;
	};
}