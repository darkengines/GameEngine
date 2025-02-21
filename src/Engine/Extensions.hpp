#pragma once
#include <boost/di.hpp>

#include "../Engine/EngineState.hpp"
#include "../Devices/Extensions.hpp"
#include "../Extensions.hpp"
#include "DescriptorSetLayouts.hpp"
#include <fruit/fruit.h>

namespace drk::Engine {
fruit::Component<EngineState, DescriptorSetLayouts, DescriptorSetLayoutCache, DescriptorSetAllocator> addEngine() {
	return fruit::createComponent()
		.registerConstructor<DescriptorSetLayouts(
			DescriptorSetLayoutCache& descriptorSetLayoutCache
		)>()
		.registerConstructor<EngineState(
			const Devices::DeviceContext&,
			entt::registry&,
			DescriptorSetLayoutCache&,
			DescriptorSetAllocator&,
			const DescriptorSetLayouts&
		)>()
		.registerConstructor<DescriptorSetLayoutCache(
			const Devices::DeviceContext&
		)>()
		.registerConstructor<DescriptorSetAllocator(
			const Devices::DeviceContext&
		)>()
		.install(drk::addRegistry)
		.install(drk::Devices::addDevices);
}
auto AddEngine() {
	return boost::di::make_injector(
		boost::di::bind<EngineState>.to<EngineState>(),
		boost::di::bind<DescriptorSetLayoutCache>.to<DescriptorSetLayoutCache>(),
		boost::di::bind<DescriptorSetAllocator>.to<DescriptorSetAllocator>(),
		boost::di::bind<DescriptorSetLayouts>.to<DescriptorSetLayouts>()
	);
}
}  // namespace drk::Engine