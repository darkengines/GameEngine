#pragma once

#include "../Systems/System.hpp"
#include "Models/Line.hpp"
#include "Components/Line.hpp"
#include "../Draws/DrawSystem.hpp"

namespace drk::Lines {

	class LineSystem : public Systems::System<Models::Line, Components::Line>, public Draws::DrawSystem {
	public:
		LineSystem(Engine::EngineState& engineState, entt::registry& registry, Devices::DeviceContext& deviceContext);
		~LineSystem();
		void UpdateDraw(entt::entity drawEntity, int drawIndex);
		void UpdateShadowDraw(entt::entity shadowDrawEntity, int drawIndex);
		bool EmitDraws();
		void UpdateDraws() { throw std::runtime_error("Not supported"); }
		void Update(Models::Line& model, const Components::Line& line) override;
		void CreateResources();
	protected:
		Devices::DeviceContext& deviceContext;
		Devices::BufferView lineVertexBufferView;
		Devices::BufferView lineIndexBufferView;
	};

}