#pragma once
#include "../Systems/System.hpp"
#include "Models/Point.hpp"
#include "Components/Point.hpp"
#include "Models/PointDraw.hpp"
#include "../Objects/Object.hpp"

namespace drk::Points {
	class PointDrawSystem
: public Systems::DrawSystem<Models::PointDraw, Stores::StoreItem<Models::Point>, Stores::StoreItem<Objects::Object>> {
	public:
		void Update(
			Models::PointDraw& model,
			const Stores::StoreItem<Models::Point>& pointStoreItem,
			const Stores::StoreItem<Objects::Object>& objectStoreItem
		) override;
	};
}
