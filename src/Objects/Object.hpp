#pragma once

#include <entt/entity/entity.hpp>
#include "../Spatials/Spatial.hpp"

namespace drk::Objects {
	struct Object {
		Spatials::Spatial spatial;
	};
}