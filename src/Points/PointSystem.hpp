#pragma once

#include "../Systems/System.hpp"
#include "Models/Point.hpp"
#include "Components/Point.hpp"

namespace drk::Points {

	class PointSystem : public Systems::System<Models::Point, Components::Point> {
	public:
		void Update(Models::Point& model, const Components::Point& point) override;
	};

}