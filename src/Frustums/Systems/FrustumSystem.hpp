#pragma once

#include "../../Systems/StorageSystem.hpp"
#include "../Components/Frustum.hpp"
#include "../Models/Frustum.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"

namespace drk::Frustums::Systems {
	class FrustumSystem :
		public drk::Systems::StorageSystem<Models::Frustum, Components::Frustum>,
		public drk::Draws::Systems::IDrawSystem {
	protected:
		const Devices::DeviceContext& deviceContext;
		Devices::BufferView vertexBufferView;
		Devices::BufferView indexBufferView;
	public:
		FrustumSystem(
			Engine::EngineState& engineState,
			entt::registry& registry,
			const Devices::DeviceContext& deviceContext
		);
		~FrustumSystem();
		void update(Models::Frustum& frustumModel, const Components::Frustum& frustumComponent) override;
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
		void processDirty();
		void emitDraws() override;
		void createResources();
	};
}