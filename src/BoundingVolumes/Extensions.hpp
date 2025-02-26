#pragma once

#include "./Pipelines/BoundingVolumePipeline.hpp"
#include "./Systems/AxisAlignedBoundingBoxSystem.hpp"

namespace drk::BoundingVolumes {
fruit::Component<std::function<std::unique_ptr<Pipelines::BoundingVolumePipeline>()>, Systems::AxisAlignedBoundingBoxSystem> addBoundingVolumes() {
	return fruit::createComponent()
		.registerFactory<std::unique_ptr<Pipelines::BoundingVolumePipeline>(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		)>([](
			   const Devices::DeviceContext& deviceContext,
			   Engine::EngineState& engineState,
			   const Engine::DescriptorSetLayouts& descriptorSetLayouts
		   ) {
			return std::make_unique<Pipelines::BoundingVolumePipeline>(
				deviceContext, 
				engineState, 
				descriptorSetLayouts
			);
		})
		.registerConstructor<Systems::AxisAlignedBoundingBoxSystem(
			Engine::EngineState&,
			entt::registry&,
			const Devices::DeviceContext&
		)>()
		.install(Engine::addEngine)
		.install(Devices::addDevices)
		.install(drk::addRegistry)
		.addMultibinding<drk::Systems::IStorageSystem, Systems::AxisAlignedBoundingBoxSystem>()
		.addMultibinding<drk::Draws::Systems::IDrawSystem, Systems::AxisAlignedBoundingBoxSystem>();
	;
}
auto AddBoundingVolumes() {
	return boost::di::make_injector(
		boost::di::bind<Pipelines::BoundingVolumePipeline>.to<Pipelines::BoundingVolumePipeline>(),
		boost::di::bind<Systems::AxisAlignedBoundingBoxSystem>.to<Systems::AxisAlignedBoundingBoxSystem>()
	);
}
}  // namespace drk::BoundingVolumes