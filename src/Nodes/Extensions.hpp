#pragma once

#include <boost/di.hpp>
#include "Systems/NodeSystem.hpp"

namespace drk::Nodes {
fruit::Component<Systems::NodeSystem> addObjects() {
	return fruit::createComponent()
		.registerConstructor<Systems::NodeSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		)>()
		.install(Devices::addDevices)
		.install(Engine::addEngine)
		.install(drk::addRegistry)
		.addMultibinding<drk::Systems::IStorageSystem, Systems::NodeSystem>();
}
	auto AddObjects() {
		return boost::di::make_injector(
			boost::di::bind<Systems::NodeSystem>.to<Systems::NodeSystem>()
		);
	}
}