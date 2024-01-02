#pragma once

#include "../../Systems/System.hpp"
#include "../Models/Point.hpp"
#include "../Components/Point.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"

namespace drk::Points::Systems {

class PointSystem : public drk::Systems::System<Models::Point, Components::Point>, public Draws::Systems::DrawSystem {
	public:
		PointSystem(Engine::EngineState& engineState, entt::registry& registry, Devices::DeviceContext& deviceContext);
		~PointSystem();
		void updateDraw(entt::entity drawEntity, int drawIndex);
		void emitDraws();
		void updateDraws() { throw std::runtime_error("Not supported"); }
		void update(Models::Point& model, const Components::Point& point) override;
		void createResources();
	protected:
		Devices::DeviceContext& deviceContext;
		Devices::BufferView pointVertexBufferView;
		Devices::BufferView pointIndexBufferView;
	};

}