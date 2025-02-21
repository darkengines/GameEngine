#pragma once

#include <fruit/fruit.h>

#include <boost/di.hpp>
#include <entt/entt.hpp>

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "./Systems/DirectionalLightSystem.hpp"
#include "./Systems/LightPerspectiveSystem.hpp"
#include "./Systems/LightSystem.hpp"
#include "./Systems/PointLightSystem.hpp"
#include "./Systems/ShadowMappingSystem.hpp"
#include "./Systems/SpotlightSystem.hpp"

namespace drk::Lights {

fruit::Component<
	Systems::LightSystem,
	Systems::ShadowMappingSystem,
	Systems::SpotlightSystem,
	Systems::DirectionalLightSystem,
	Systems::PointLightSystem,
	Systems::LightPerspectiveSystem
>
addLights() {
	return fruit::createComponent()
		.registerConstructor<Systems::LightSystem(
			Engine::EngineState&,
			entt::registry&
		)>()
		.registerConstructor<Systems::ShadowMappingSystem(
			Devices::DeviceContext&,
			Engine::EngineState&
		)>()
		.registerConstructor<Systems::SpotlightSystem(
			Engine::EngineState&,
			entt::registry&
		)>()
		.registerConstructor<Systems::DirectionalLightSystem(
			const Devices::DeviceContext&,
			Engine::EngineState&,
			entt::registry&
		)>()
		.registerConstructor<Systems::PointLightSystem(
			const Devices::DeviceContext&,
			Engine::EngineState&,
			entt::registry&,
			Systems::ShadowMappingSystem&
		)>()
		.registerConstructor<Systems::LightPerspectiveSystem(
			const Devices::DeviceContext&,
			Engine::EngineState&,
			entt::registry&,
			Systems::ShadowMappingSystem&
		)>()
		.install(drk::addRegistry)
		.install(Devices::addDevices)
		.install(Engine::addEngine);
}
auto AddLights() {
	return boost::di::make_injector(
		boost::di::bind<Systems::LightSystem>.to<Systems::LightSystem>(),
		boost::di::bind<Systems::PointLightSystem>.to<Systems::PointLightSystem>(),
		boost::di::bind<Systems::SpotlightSystem>.to<Systems::SpotlightSystem>(),
		boost::di::bind<Systems::DirectionalLightSystem>.to<Systems::DirectionalLightSystem>(),
		boost::di::bind<Systems::LightPerspectiveSystem>.to<Systems::LightPerspectiveSystem>(),
		boost::di::bind<Systems::ShadowMappingSystem>.to<Systems::ShadowMappingSystem>()
	);
}
}  // namespace drk::Lights