#pragma once

#include <boost/di.hpp>

#include "Systems/RelativeSpatialSystem.hpp"
#include "Systems/SpatialSystem.hpp"

namespace drk::Spatials {
fruit::Component<Systems::SpatialSystem, Systems::RelativeSpatialSystem> addSpatials() {
	return fruit::createComponent()
		.registerConstructor<Systems::SpatialSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		)>()
		.registerConstructor<Systems::RelativeSpatialSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		)>()
		.install(Devices::addDevices)
		.install(Engine::addEngine)
		.install(drk::addRegistry)
		.addMultibinding<drk::Systems::IStorageSystem, Systems::SpatialSystem>()
		.addMultibinding<drk::Systems::IStorageSystem, Systems::RelativeSpatialSystem>();
}
auto AddSpatials() {
	return boost::di::make_injector(
		boost::di::bind<Systems::SpatialSystem>.to<Systems::SpatialSystem>(),
		boost::di::bind<Systems::RelativeSpatialSystem>.to<Systems::RelativeSpatialSystem>()
	);
}
}  // namespace drk::Spatials