#pragma once

#include <boost/di.hpp>

#include "Pipelines/PointPrimitivePipeline.hpp"
#include "Systems/PointSystem.hpp"

namespace drk::Points {
fruit::Component<
	Systems::PointSystem, 
	std::function<std::unique_ptr<Points::Pipelines::PointPrimitivePipeline>()>
	> addPoints() {
	return fruit::createComponent()
		.registerConstructor<Systems::PointSystem(
			Engine::EngineState &,
			entt::registry &,
			Devices::DeviceContext &
		)>()
		.registerFactory<std::unique_ptr<Points::Pipelines::PointPrimitivePipeline>(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		)>([](
			   const Devices::DeviceContext& deviceContext,
			   Engine::EngineState& engineState,
			   const Engine::DescriptorSetLayouts& descriptorSetLayouts
		   ) {
			return std::make_unique<Points::Pipelines::PointPrimitivePipeline>(
				deviceContext, 
				engineState, 
				descriptorSetLayouts
			);
		})
		.install(Engine::addEngine)
		.install(Devices::addDevices)
		.install(drk::addRegistry)
		.addMultibinding<drk::Systems::IStorageSystem, Systems::PointSystem>()
		.addMultibinding<drk::Draws::Systems::IDrawSystem, Systems::PointSystem>();
}
auto AddPoints() {
	return boost::di::make_injector(
		boost::di::bind<Systems::PointSystem>.to<Systems::PointSystem>(),
		boost::di::bind<Points::Pipelines::PointPrimitivePipeline>.to<Points::Pipelines::PointPrimitivePipeline>().in(
			boost::di::unique
		)
	);
}
}  // namespace drk::Points