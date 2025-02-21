#pragma once

#include <boost/di.hpp>

#include "Pipelines/LinePipeline.hpp"
#include "Systems/LineSystem.hpp"

namespace drk::Lines {
fruit::Component<Systems::LineSystem, std::function<std::unique_ptr<Lines::Pipelines::LinePipeline>()>> addLines() {
	return fruit::createComponent()
		.registerConstructor<Systems::LineSystem(
			Engine::EngineState &,
			entt::registry &,
			Devices::DeviceContext &
		)>()
		.registerFactory<std::unique_ptr<Lines::Pipelines::LinePipeline>(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		)>([](
			   const Devices::DeviceContext& deviceContext,
			   Engine::EngineState& engineState,
			   const Engine::DescriptorSetLayouts& descriptorSetLayouts
		   ) {
			return std::make_unique<Lines::Pipelines::LinePipeline>(
				deviceContext, 
				engineState, 
				descriptorSetLayouts
			);
		})
		.install(Engine::addEngine)
		.install(Devices::addDevices)
		.install(drk::addRegistry);
}
auto AddLines() {
	return boost::di::make_injector(
		boost::di::bind<Systems::LineSystem>.to<Systems::LineSystem>(),
		boost::di::bind<Lines::Pipelines::LinePipeline>.to<Lines::Pipelines::LinePipeline>().in(boost::di::unique)
	);
}
}  // namespace drk::Lines