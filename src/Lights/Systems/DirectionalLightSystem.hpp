#pragma once

#include "../../Systems/System.hpp"
#include "../Models/DirectionalLight.hpp"
#include "../Components/DirectionalLight.hpp"
#include "../Models/Light.hpp"
#include "../../Spatials/Models/Spatial.hpp"

namespace drk::Lights::Systems {
	class DirectionalLightSystem : public drk::Systems::System<
		Models::DirectionalLight,
		Components::DirectionalLight,
		Stores::StoreItem<Models::Light>,
		Stores::StoreItem<Spatials::Models::Spatial>
	> {
	public:
		DirectionalLightSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void Update(
			Models::DirectionalLight& model,
			const Components::DirectionalLight& directionalLight,
			const Stores::StoreItem<Models::Light>& lightStoreItem,
			const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
		);
		void ProcessDirtyItems();
	};
}