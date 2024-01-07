#include <entt/entt.hpp>
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::BoundingVolumes::Components {
	struct Draw {
		entt::entity boundingVolumeEntity;
		entt::entity cameraEntity;
	};
}