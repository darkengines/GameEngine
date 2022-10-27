#pragma once

#include <boost/di.hpp>
#include "DeviceContext.hpp"
#include "../Configuration/Extensions.hpp"
#include "Device.hpp"
#include "VulkanInstanceConfiguration.hpp"
#include "../Windows/Window.hpp"

namespace drk::Devices {
	static auto AddDevices() {
		return boost::di::make_injector(
			boost::di::bind<DeviceContext>.to<DeviceContext>()
		);
	}
}
