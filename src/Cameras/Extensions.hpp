#pragma once

#include <fruit/fruit.h>

#include <boost/di.hpp>

#include "Systems/CameraSystem.hpp"
#include "../Devices/Extensions.hpp"
#include "../Engine/Extensions.hpp"
#include "../Extensions.hpp"

namespace drk::Cameras {
fruit::Component<Systems::CameraSystem> addCameras() {
	return fruit::createComponent()
		.registerConstructor<Systems::CameraSystem(
			const Devices::DeviceContext&,
			Engine::EngineState&,
			entt::registry&
		)>()
		.install(Devices::addDevices)
		.install(Engine::addEngine)
		.install(drk::addRegistry);
}
auto AddCameras() {
	return boost::di::make_injector(
		boost::di::bind<Systems::CameraSystem>.to<Systems::CameraSystem>()
	);
}
}  // namespace drk::Cameras