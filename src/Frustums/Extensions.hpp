#pragma once

#include "./Pipelines/FrustumPipeline.hpp"
#include "./Systems/FrustumSystem.hpp"

namespace drk::Frustums {
fruit::Component<std::function<std::unique_ptr<Pipelines::FrustumPipeline>()>, Systems::FrustumSystem> addFrustums() {
	return fruit::createComponent()
		.registerFactory<std::unique_ptr<Pipelines::FrustumPipeline>(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		)>([](
			   const Devices::DeviceContext& deviceContext,
			   Engine::EngineState& engineState,
			   const Engine::DescriptorSetLayouts& descriptorSetLayouts
		   ) {
			return std::make_unique<Pipelines::FrustumPipeline>(
				deviceContext, 
				engineState, 
				descriptorSetLayouts
			);
		})
		.registerConstructor<Systems::FrustumSystem(
			Engine::EngineState&,
			entt::registry&,
			const Devices::DeviceContext& deviceContext
		)>()
		.install(Engine::addEngine)
		.install(Devices::addDevices)
		.install(drk::addRegistry);
}
auto AddFrustums() {
	return boost::di::make_injector(
		boost::di::bind<Pipelines::FrustumPipeline>.to<Pipelines::FrustumPipeline>(),
		boost::di::bind<Systems::FrustumSystem>.to<Systems::FrustumSystem>()
	);
}
}  // namespace drk::Frustums