#pragma once
#include <fruit/fruit.h>
#include "../Devices/Extensions.hpp"
#include "Resources/BufferResourceManager.hpp"

namespace drk::Buffers {
fruit::Component<Resources::BufferResourceManager> addBuffers() {
	return fruit::createComponent()
		.registerConstructor<Resources::BufferResourceManager(Devices::DeviceContext & deviceContext)>()
		.install(Devices::addDevices);
}
}  // namespace drk::Meshes