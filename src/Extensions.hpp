#pragma once

#include <fruit/fruit.h>
#include <entt/entt.hpp>

namespace drk {
fruit::Component<entt::registry> addRegistry() {
	return fruit::createComponent()
		.registerProvider([]() { return entt::registry(); });
}
}  // namespace drk