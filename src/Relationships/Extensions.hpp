#pragma once

#include <boost/di.hpp>
#include "Systems/RelationshipSystem.hpp"

namespace drk::Relationships {
auto addRelationships() {
	return fruit::createComponent()
		.registerConstructor<Systems::RelationshipSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		)>();
}
	auto AddRelationships() {
		return boost::di::make_injector(
			boost::di::bind<Systems::RelationshipSystem>.to<Systems::RelationshipSystem>()
		);
	}
}