#pragma once

#include "entt/entity/entity.hpp"
#include "DrawVertexBufferInfo.hpp"

namespace drk::Draws {
	class DrawSystem {
	public:
		virtual void UpdateDraw(entt::entity drawEntity, int drawIndex) = 0;
		virtual void UpdateDraws() = 0;
		virtual bool EmitDraws() = 0;
		virtual DrawVertexBufferInfo GetVertexBufferInfo(entt::entity drawEntity) = 0;
	};
}