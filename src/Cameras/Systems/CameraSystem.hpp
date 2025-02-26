#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Components/Camera.hpp"
#include "../Models/Camera.hpp"
#include "../../Systems/StorageSystem.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Spatials/Models/RelativeSpatial.hpp"
#include "../../Spatials/Components/Spatial.hpp"

namespace drk::Cameras::Systems {
	class CameraSystem : public drk::Systems::StorageSystem<
		Models::Camera, 
		Components::Camera, 
		Stores::StoreItem<Spatials::Models::Spatial>,
        Stores::StoreItem<Spatials::Models::RelativeSpatial>
	> {
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		CameraSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void update(
			Models::Camera& cameraModel, 
			const Components::Camera& camera,
            const Stores::StoreItem<Spatials::Models::Spatial>& absoluteSpatialStoreItem,
            const Stores::StoreItem<Spatials::Models::RelativeSpatial>& relativeSpatialStoreItem
		);
		void processDirtyItems();

		entt::entity createCamera(
			glm::vec4 position,
			glm::vec4 front,
			glm::vec4 up,
			float verticalFov,
			float aspectRatio,
			float near,
			float far
		) const;
	};
}
