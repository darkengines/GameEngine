#pragma once
#include <boost/di.hpp>

#include "../Engine/EngineState.hpp"
#include "../Lights/Extensions.hpp"

namespace drk::Graphics {
fruit::Component<GlobalSystem, Graphics> addGraphics() {
	return fruit::createComponent()
		.registerConstructor<GlobalSystem(
			Engine::EngineState & engineState,
			entt::registry & registry,
			Lights::Systems::DirectionalLightSystem & directionalLightSystem,
			Lights::Systems::SpotlightSystem & spotlightSystem,
			Lights::Systems::PointLightSystem & pointLightSystem
		)>()
		.registerConstructor<Graphics(
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Windows::Window& windows
		)>()
		.install(Devices::addDevices)
		.install(Windows::addWindows)
		.install(drk::addRegistry)
		.install(Engine::addEngine)
		.install(Lights::addLights);
}
auto AddGraphics() {
	return boost::di::make_injector(
		boost::di::bind<GlobalSystem>.to<GlobalSystem>(),
		boost::di::bind<Graphics>.to<Graphics>()
	);
}
}  // namespace drk::Graphics