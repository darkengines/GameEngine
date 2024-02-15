#pragma once

#include "../../Systems/System.hpp"
#include "../Components/Frustum.hpp"
#include "../Models/Frustum.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"

namespace drk::Frustums::Systems {
	class FrustumSystem :
		public drk::Systems::System<Models::Frustum, Components::Frustum>,
		drk::Draws::Systems::DrawSystem {
	protected:
		Devices::DeviceContext deviceContext;
		Devices::BufferView vertexBufferView;
		Devices::BufferView indexBufferView;
	public:
		FrustumSystem(
			Engine::EngineState& engineState,
			entt::registry& registry,
			Devices::DeviceContext& deviceContext
		);
		~FrustumSystem();
		void update(Models::Frustum& frustumModel, const Components::Frustum& frustumComponent) override;
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
		void processDirty();
		void emitDraws() override;
		void createResources();
	};
}