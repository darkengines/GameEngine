#pragma once

#include "../../Systems/System.hpp"
#include "../Models/Line.hpp"
#include "../Components/Line.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"

namespace drk::Lines::Systems {

class LineSystem : public drk::Systems::System<Models::Line, Components::Line>, public Draws::Systems::DrawSystem {
	public:
		LineSystem(Engine::EngineState& engineState, entt::registry& registry, Devices::DeviceContext& deviceContext);
		~LineSystem();
		void updateDraw(entt::entity drawEntity, int drawIndex);
		void updateShadowDraw(entt::entity shadowDrawEntity, int drawIndex);
		void emitDraws();
		void updateDraws() { throw std::runtime_error("Not supported"); }
		void update(Models::Line& model, const Components::Line& line) override;
		void createResources();
	protected:
		Devices::DeviceContext& deviceContext;
		Devices::BufferView lineVertexBufferView;
		Devices::BufferView lineIndexBufferView;
	};

}