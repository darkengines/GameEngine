#pragma once

#include <entt/entity/entity.hpp>

namespace drk::Meshes::Components {
	struct Mesh {
		bool hasTangent;
		bool hasBitTangent;
	};
}