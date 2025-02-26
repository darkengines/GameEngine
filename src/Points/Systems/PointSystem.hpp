#pragma once

#include "../../Systems/StorageSystem.hpp"
#include "../Models/Point.hpp"
#include "../Components/Point.hpp"
#include "../../Draws/Systems/IDrawSystem.hpp"

namespace drk::Points::Systems {

	class PointSystem
		: public drk::Systems::StorageSystem<Models::Point, Components::Point>, public Draws::Systems::IDrawSystem {
	public:
		PointSystem(Engine::EngineState& engineState, entt::registry& registry, Devices::DeviceContext& deviceContext);
		~PointSystem();
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
		void emitDraws() override;
		void update(Models::Point& model, const Components::Point& point) override;
		void createResources();
	protected:
		Devices::DeviceContext& deviceContext;
		Devices::BufferView pointVertexBufferView;
		Devices::BufferView pointIndexBufferView;
	};

}