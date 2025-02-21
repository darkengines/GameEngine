#pragma once

#include <boost/di.hpp>

#include "../Configuration/Configuration.hpp"
#include "../Configuration/Extensions.hpp"
#include "../Windows/Extensions.hpp"
#include "../Windows/Window.hpp"
#include "Device.hpp"
#include "DeviceContext.hpp"
#include "VulkanInstanceConfiguration.hpp"

namespace drk::Devices {
fruit::Component<DeviceContext>
addDevices() {
	return fruit::createComponent()
		.registerConstructor<DeviceContext(
			const Configuration::Configuration& configuration,
			const Windows::Window& window
		)>()
		.install(Windows::addWindows)
		.install(Configuration::addConfiguration);
}
static auto AddDevices() {
	return boost::di::make_injector(
		boost::di::bind<DeviceContext>.to<DeviceContext>()
	);
}
}  // namespace drk::Devices
