#pragma once

#include <entt/entity/entity.hpp>

namespace drk::Graphics {
	struct Draw {
		entt::entity meshEntity;
		entt::entity objectEntity;
		entt::entity drawSetEntity;
	};
}