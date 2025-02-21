#pragma once
#include <boost/di.hpp>

#include "../Buffers/Extensions.hpp"
#include "../Cameras/Extensions.hpp"
#include "../Controllers/Extensions.hpp"
#include "../Devices/Extensions.hpp"
#include "../Engine/Extensions.hpp"
#include "../Extensions.hpp"
#include "../Graphics/Extentions.hpp"
#include "../Lights/Extensions.hpp"
#include "../Loaders/Extensions.hpp"
#include "../Materials/Extensions.hpp"
#include "../Meshes/Extensions.hpp"
#include "../Nodes/Extensions.hpp"
#include "../Scenes/Extensions.hpp"
#include "../Spatials/Extensions.hpp"
#include "../Textures/Extensions.hpp"
#include "../UserInterfaces/Extensions.hpp"
#include "../Windows/Extensions.hpp"
#include "../Animations/Extensions.hpp"
#include "Application.hpp"
#include "Root.hpp"

namespace drk::Applications {
fruit::Component<Root, entt::registry, Loaders::AssimpLoader> addRoot() {
	return fruit::createComponent()
		.registerConstructor<Root(
			const Windows::Window& window,
			Engine::EngineState& engineState,
			const Devices::DeviceContext& deviceContext,
			Textures::Systems::TextureSystem& textureSystem,
			Materials::Systems::MaterialSystem& materialSystem,
			Meshes::Systems::MeshSystem& meshSystem,
			Meshes::Systems::MeshShadowSystem& meshShadowSystem,
			Spatials::Systems::SpatialSystem& spatialSystem,
			Spatials::Systems::RelativeSpatialSystem& relativeSpatialSystem,
			Nodes::Systems::NodeSystem& objectSystem,
			Cameras::Systems::CameraSystem& cameraSystem,
			Graphics::GlobalSystem& globalSystem,
			const Loaders::AssimpLoader& loader,
			Graphics::Graphics& graphics,
			Controllers::FlyCamController& flyCamController,
			UserInterfaces::UserInterface& userInterface,
			entt::registry& registry,
			UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer,
			Scenes::Renderers::SceneRenderer& sceneRenderer,
			Scenes::Systems::SceneSystem& sceneSystem,
			Points::Systems::PointSystem& pointSystem,
			BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem& axisAlignedBoundingBoxSystem,
			Frustums::Systems::FrustumSystem& frustumSystem,
			Lines::Systems::LineSystem& lineSystem,
			Lights::Systems::LightSystem& lightSystem,
			Lights::Systems::PointLightSystem& pointLightSystem,
			Lights::Systems::DirectionalLightSystem& directionalLightSystem,
			Lights::Systems::SpotlightSystem& spotlightSystem,
			Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem,
			Animations::Systems::AnimationSystem& animationSystem,
			Animations::Systems::BoneMeshSystem& boneSystem,
			Animations::Systems::BoneSpatialSystem& boneSpatialSystem
		)>()
		.install(drk::addRegistry)
		.install(drk::Windows::addWindows)
		.install(drk::Engine::addEngine)
		.install(drk::Devices::addDevices)
		.install(drk::Textures::addTextures)
		.install(drk::Materials::addMaterials)
		.install(drk::Meshes::addMeshes)
		.install(drk::Buffers::addBuffers)
		.install(drk::Spatials::addSpatials)
		.install(drk::Nodes::addObjects)
		.install(drk::Cameras::addCameras)
		.install(drk::Graphics::addGraphics)
		.install(drk::Loaders::addLoaders)
		.install(drk::Controllers::addControllers)
		.install(drk::UserInterfaces::addUserInterfaces)
		.install(drk::Scenes::addScenes)
		.install(drk::Lights::addLights)
		.install(drk::Points::addPoints)
		.install(drk::Frustums::addFrustums)
		.install(drk::BoundingVolumes::addBoundingVolumes)
		.install(drk::Lines::addLines)
		.install(drk::Animations::addAnimation);
}
// fruit::Component<Application> addApplications() {
//	return fruit::createComponent()
//		.registerConstructor<Application(
//			const Windows::Window& window,
//			Engine::EngineState& engineState,
//			const Devices::DeviceContext& deviceContext,
//			Textures::Systems::TextureSystem& textureSystem,
//			Materials::Systems::MaterialSystem& materialSystem,
//			Meshes::Systems::MeshSystem& meshSystem,
//			Meshes::Systems::MeshShadowSystem& meshShadowSystem,
//			Spatials::Systems::SpatialSystem& spatialSystem,
//			Spatials::Systems::RelativeSpatialSystem& relativeSpatialSystem,
//			Nodes::Systems::NodeSystem& objectSystem,
//			Cameras::Systems::CameraSystem& cameraSystem,
//			Graphics::GlobalSystem& globalSystem,
//			const Loaders::AssimpLoader& loader,
//			Graphics::Graphics& graphics,
//			Controllers::FlyCamController& flyCamController,
//			UserInterfaces::UserInterface& userInterface,
//			entt::registry& registry,
//			UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer,
//			Scenes::Renderers::SceneRenderer& sceneRenderer,
//			Scenes::Systems::SceneSystem& sceneSystem,
//			Points::Systems::PointSystem& pointSystem,
//			BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem& axisAlignedBoundingBoxSystem,
//			Frustums::Systems::FrustumSystem& frustumSystem,
//			Lines::Systems::LineSystem&,
//			Lights::Systems::LightSystem& lightSystem,
//			Lights::Systems::PointLightSystem& pointLightSystem,
//			Lights::Systems::DirectionalLightSystem& directionalLightSystem,
//			Lights::Systems::SpotlightSystem& spotlightSystem,
//			Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem,
//			Animations::Systems::AnimationSystem& animationSystem,
//			Animations::Systems::BoneMeshSystem& boneSystem,
//			Animations::Systems::BoneSpatialSystem& boneSpatialSystem,
//			UserInterfaces::AssetExplorer& assetExplorer
//		)>();
// }
auto AddApplications() {
	return boost::di::make_injector(
		boost::di::bind<Application>.to<Application>()
	);
}
}  // namespace drk::Applications
