#pragma once

#include <boost/di.hpp>
#include <fruit/fruit.h>
#include "Systems/TextureSystem.hpp"
#include "../Devices/Extensions.hpp"
#include "../Extensions.hpp"
#include "../Engine/Extensions.hpp"

namespace drk::Textures {
fruit::Component<Systems::TextureSystem> addTextures() {
	return fruit::createComponent()
		.registerConstructor<Systems::TextureSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		)>()
		.install(Devices::addDevices)
		.install(Engine::addEngine)
		.install(drk::addRegistry);
}
auto AddTextures() {
	return boost::di::make_injector(
		boost::di::bind<Systems::TextureSystem>.to<Systems::TextureSystem>()
	);
}
}  // namespace drk::Textures