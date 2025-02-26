#pragma once

#include <fruit/fruit.h>

#include <boost/di.hpp>

#include "Pipelines/SkinningPipeline.hpp"
#include "Resources/AnimationResourceManager.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/BoneMeshSystem.hpp"
#include "Systems/BoneSpatialSystem.hpp"
#include "../Devices/Extensions.hpp"
#include "../Engine/Extensions.hpp"
#include "../Extensions.hpp"
#include "../Configuration/Extensions.hpp"

namespace drk::Animations {
fruit::Component<Systems::AnimationSystem, Systems::BoneMeshSystem, Systems::BoneSpatialSystem> addAnimation() {
	return fruit::createComponent()
		.registerConstructor<
			Systems::AnimationSystem(
				entt::registry&,
				Devices::DeviceContext&,
				Engine::EngineState&,
				Animations::Resources::AnimationResourceManager&,
				Pipelines::SkinningPipeline&
			)>()
		.registerConstructor<
			Resources::AnimationResourceManager(
				const Configuration::Configuration& configuration,
				Devices::DeviceContext& deviceContext,
				Engine::DescriptorSetAllocator& descriptorSetAllocator,
				Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache
			)>()
		.registerConstructor<
			Pipelines::SkinningPipeline(
				const Devices::DeviceContext& deviceContext,
				Engine::EngineState& engineState,
				Animations::Resources::AnimationResourceManager& animationResourceManager,
				Engine::DescriptorSetLayouts& descriptorSetLayouts
			)>()
		.registerConstructor<
			Systems::BoneMeshSystem(
				Engine::EngineState & engineState,
				entt::registry & registry
			)>()
		.registerConstructor<
			Systems::BoneSpatialSystem(
				Engine::EngineState & engineState,
				entt::registry & registry
			)>()
		.install(Engine::addEngine)
		.install(Devices::addDevices)
		.install(drk::addRegistry)
		.install(drk::Configuration::addConfiguration)
		.addMultibinding<drk::Systems::IStorageSystem, Systems::BoneMeshSystem>()
		.addMultibinding<drk::Systems::IStorageSystem, Systems::BoneSpatialSystem>();
}

auto AddAnimations() {
	return boost::di::make_injector(
		boost::di::bind<Systems::AnimationSystem>.to<Systems::AnimationSystem>(),
		boost::di::bind<Resources::AnimationResourceManager>.to<Resources::AnimationResourceManager>(),
		boost::di::bind<Pipelines::SkinningPipeline>.to<Pipelines::SkinningPipeline>(),
		boost::di::bind<Systems::BoneMeshSystem>.to<Systems::BoneMeshSystem>(),
		boost::di::bind<Systems::BoneSpatialSystem>.to<Systems::BoneSpatialSystem>()
	);
}
}  // namespace drk::Animations
