#pragma once

#include "../../Systems/StorageSystem.hpp"
#include "../Models/Line.hpp"
#include "../Components/Line.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"

namespace drk::Lines::Systems {

	class LineSystem : public drk::Systems::StorageSystem<Models::Line, Components::Line>, public Draws::Systems::IDrawSystem {
	public:
		LineSystem(Engine::EngineState& engineState, entt::registry& registry, Devices::DeviceContext& deviceContext);
		~LineSystem();
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
		void updateShadowDraw(entt::entity shadowDrawEntity, int drawIndex);
		void emitDraws() override;
		void update(Models::Line& model, const Components::Line& line) override;
		void createResources();
	protected:
		Devices::DeviceContext& deviceContext;
		Devices::BufferView lineVertexBufferView;
		Devices::BufferView lineIndexBufferView;
	};

}