#pragma once

#include "../Systems/System.hpp"
#include "Models/Point.hpp"
#include "Components/Point.hpp"
#include "../Draws/DrawSystem.hpp"

namespace drk::Points {

	class PointSystem : public Systems::System<Models::Point, Components::Point>, public Draws::DrawSystem {
	public:
		void Update(Models::Point& model, const Components::Point& point) override;
		void EmitDraws();
		void UpdateDraw(entt::entity drawEntity, int drawIndex);
	};

}