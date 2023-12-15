
#pragma once
#include "Models/PointLight.hpp"
#include "Components/PointLight.hpp"
#include "../Systems/System.hpp"
#include "../Spatials/Components/Spatial.hpp"

namespace drk::Lights {
	class PointLightSystem : public Systems::System<Models::PointLight, Components::PointLight, drk::Spatials::Components::Spatial> {
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		PointLightSystem(const Devices::DeviceContext& deviceContext, Engine::EngineState& engineState, entt::registry& registry);
		virtual void Update(Models::PointLight& model, const Components::PointLight& component, const drk::Spatials::Components::Spatial& spatial) override;
	};
}