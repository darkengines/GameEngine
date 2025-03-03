#pragma once
#include <boost/di.hpp>
#include "Systems/SceneSystem.hpp"
#include "Renderers/SceneRenderer.hpp"
#include "Renderers/ShadowSceneRenderer.hpp"
#include "../Points/Extensions.hpp"
#include "../Lines/Extensions.hpp"
#include "../Frustums/Extensions.hpp"
#include "../BoundingVolumes/Extensions.hpp"
#include <fruit/fruit.h>

namespace drk::Scenes {
fruit::Component<Systems::SceneSystem, Renderers::SceneRenderer> addScenes() {
	return fruit::createComponent()
		.registerConstructor<Systems::SceneSystem(entt::registry & registry, Engine::EngineState & engineState)>()
		.registerConstructor<Renderers::SceneRenderer(
			Engine::EngineState & engineState,
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::function<std::unique_ptr<Meshes::Pipelines::MeshPipeline>()> meshPipelineFactory,
			std::function<std::unique_ptr<Points::Pipelines::PointPrimitivePipeline>()> pointPrimitivePipelineFactory,
			std::function<std::unique_ptr<Lines::Pipelines::LinePipeline>()> linePipelineFactory,
			std::function<std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline>()> boundingVolumePipelineFactory,
			std::function<std::unique_ptr<Frustums::Pipelines::FrustumPipeline>()> frustumPipelineFactory,
			std::function<std::unique_ptr<Renderers::ShadowSceneRenderer>()> shadowSceneRendererFactory,
			Lights::Systems::ShadowMappingSystem& shadowMappingSystem,
            const Windows::Window& window
		)>()
		.registerFactory<std::unique_ptr<Renderers::ShadowSceneRenderer>(
			const Devices::DeviceContext& deviceContext,
			entt::registry& registry,
			std::function<std::unique_ptr<Meshes::Pipelines::ShadowMeshPipeline>()> meshShadowPipelineFactory
		)>([](
			   const Devices::DeviceContext& deviceContext,
			   entt::registry& registry,
			   std::function<std::unique_ptr<Meshes::Pipelines::ShadowMeshPipeline>()> meshShadowPipelineFactory
		   ) {
			return std::make_unique<Renderers::ShadowSceneRenderer>(
				deviceContext, 
				registry, 
				meshShadowPipelineFactory
			);
		})
		.install(Engine::addEngine)
		.install(Devices::addDevices)
		.install(drk::addRegistry)
		.install(Lights::addLights)
		.install(Meshes::addMeshes)
		.install(Points::addPoints)
		.install(Lines::addLines)
		.install(BoundingVolumes::addBoundingVolumes)
		.install(Frustums::addFrustums)
        .install(Windows::addWindows);
}
	auto AddScenes() {
		return boost::di::make_injector(
			boost::di::bind<Systems::SceneSystem>.to<Systems::SceneSystem>(),
			boost::di::bind<Renderers::SceneRenderer>.to<Renderers::SceneRenderer>(),
			boost::di::bind<Renderers::ShadowSceneRenderer>.to<Renderers::ShadowSceneRenderer>()
		);
	}
}
