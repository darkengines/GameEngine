#pragma once

#include "entt/entity/entity.hpp"
#include "../Components/DrawVertexBufferInfo.hpp"

namespace drk::Draws::Systems {
	class DrawSystem {
	public:
		virtual void UpdateDraw(entt::entity drawEntity, int drawIndex) = 0;
		virtual void UpdateShadowDraw(entt::entity shadowDrawEntity, int drawIndex) = 0;
		virtual void UpdateDraws() = 0;
		virtual bool EmitDraws() = 0;
	};
}