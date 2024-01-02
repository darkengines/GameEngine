#pragma once

#include "entt/entity/entity.hpp"
#include "../Components/DrawVertexBufferInfo.hpp"

namespace drk::Draws::Systems {
	class DrawSystem {
	public:
		virtual void updateDraw(entt::entity drawEntity, int drawIndex) = 0;
		virtual void updateDraws() = 0;
		virtual void emitDraws() = 0;
	};
}