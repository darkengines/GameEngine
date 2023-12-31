
#pragma once

#include "../Models/LightPerspective.hpp"
#include "../Models/Spotlight.hpp"
#include "../Components/Spotlight.hpp"
#include "../../Systems/System.hpp"
#include "../Models/Light.hpp"
#include "../../Spatials/Models/Spatial.hpp"

namespace drk::Lights::Systems {
	class SpotlightSystem : public drk::Systems::System<
		Models::Spotlight,
		Components::Spotlight,
		Stores::StoreItem<Models::Light>,
		Stores::StoreItem<Spatials::Models::Spatial>,
		Stores::StoreItem<Models::LightPerspective>
	> {
	public:
		SpotlightSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void update(
			Models::Spotlight& model,
			const Components::Spotlight& component,
			const Stores::StoreItem<Models::Light>& lightStoreItem,
			const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem,
			const Stores::StoreItem<Models::LightPerspective>& lightPerspective
		);
		void ProcessDirtyItems();
	};
}