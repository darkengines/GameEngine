#pragma once

#include "../Systems/System.hpp"
#include "Models/Point.hpp"
#include "Components/Point.hpp"
#include "../Draws/DrawSystem.hpp"

namespace drk::Points {

	class PointSystem : public Systems::System<Models::Point, Components::Point>, public Draws::DrawSystem {
	public:
		PointSystem(Engine::EngineState& engineState, entt::registry& registry, Devices::DeviceContext& deviceContext);
		~PointSystem();
		void UpdateDraw(entt::entity drawEntity, int drawIndex);
		void UpdateShadowDraw(entt::entity shadowDrawEntity, int drawIndex);
		bool EmitDraws();
		void UpdateDraws() { throw std::runtime_error("Not supported"); }
		Draws::DrawVertexBufferInfo GetVertexBufferInfo(entt::entity drawEntity);
		void Update(Models::Point& model, const Components::Point& point) override;
		void CreateResources();
	protected:
		Devices::DeviceContext& deviceContext;
		Devices::BufferView pointVertexBufferView;
		Devices::BufferView pointIndexBufferView;
	};

}