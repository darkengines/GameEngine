#pragma once

#include <string>
#include <vector>
#include "entt/entity/entity.hpp"
#include "../Vertex.hpp"
#include "../../Materials/Components/Material.hpp"

namespace drk::Meshes::Components {
	struct MeshResource {
		std::vector<Vertex> vertices;
		std::vector<VertexIndex> indices;	
	};
}