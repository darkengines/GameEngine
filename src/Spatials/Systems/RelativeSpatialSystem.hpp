#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/RelativeSpatial.hpp"
#include "../Components/Spatial.hpp"
#include "../../Systems/System.hpp"
#include "../../Nodes/Components/Node.hpp"

namespace drk::Spatials::Systems {
	class RelativeSpatialSystem
		: public drk::Systems::System<Models::RelativeSpatial, Components::Spatial<Components::Relative>> {
	protected:
		const Devices::DeviceContext& deviceContext;

	public:
		RelativeSpatialSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		) : System(engineState, registry), deviceContext(deviceContext) {}

		void update(Models::RelativeSpatial& spatialModel, const Components::Spatial<Components::Relative>& spatial) override {
			spatialModel.position = spatial.position;
			spatialModel.rotation = spatial.rotation;
			spatialModel.scale = spatial.scale;
			spatialModel.model = spatial.model;
		}
	};
}