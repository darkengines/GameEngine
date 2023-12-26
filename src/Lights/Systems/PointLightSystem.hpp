
#pragma once
#include "../Models/PointLight.hpp"
#include "../Components/PointLight.hpp"
#include "../../Systems/System.hpp"
#include "../Models/Light.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "./ShadowMappingSystem.hpp"

namespace drk::Lights::Systems {
	class PointLightSystem : public drk::Systems::System<
		Models::PointLight,
		Components::PointLight,
		Stores::StoreItem<Models::Light>,
		Stores::StoreItem<Spatials::Models::Spatial>
	> {
	protected:
		const Devices::DeviceContext& deviceContext;
		ShadowMappingSystem& shadowMappingSystem;
	public:
		PointLightSystem(
			const Devices::DeviceContext& deviceContext, 
			Engine::EngineState& engineState, 
			entt::registry& registry,
			ShadowMappingSystem& shadowMappingSystem
		);
		virtual void Update(
			Models::PointLight& model,
			const Components::PointLight& component,
			const Stores::StoreItem<Models::Light>& lightStoreItem,
			const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
		) override;
		void ProcessDirtyItems();
	};
}