#include "GlobalSystem.hpp"

namespace drk::Graphics {

	GlobalSystem::GlobalSystem(
		Engine::EngineState& engineState,
		entt::registry& registry,
		Lights::Systems::DirectionalLightSystem& directionalLightSystem,
		Lights::Systems::SpotlightSystem& spotlightSystem,
		Lights::Systems::PointLightSystem& pointLightSystem
	) : EngineState(engineState),
		Registry(registry),
		GlobalSynchronizationState(EngineState.getFrameCount()),
		directionalLightSystem(directionalLightSystem),
		spotlightSystem(spotlightSystem),
		pointLightSystem(pointLightSystem) {

	}

	void GlobalSystem::setCamera(entt::entity cameraEntity) {
		CameraEntity = cameraEntity;
		EngineState.cameraEntity = CameraEntity;
		cameraChanged(cameraEntity);
		GlobalSynchronizationState.Reset();
	}

	void GlobalSystem::setRenderStyle(uint32_t style) {
		renderStyle = style;
		GlobalSynchronizationState.Reset();
	}

	void GlobalSystem::update() {
		if (GlobalSynchronizationState.ShouldUpdate(EngineState.getFrameIndex())) {
			const auto& camera = Registry.get<Stores::StoreItem<Cameras::Models::Camera>>(CameraEntity);

			const auto frameIndex = EngineState.getFrameIndex();
			const auto& frameStoreItem = camera.frameStoreItems[frameIndex];
			auto& frameState = EngineState.getCurrentFrameState();

			Engine::Models::Global global{};

			Stores::Models::StoreItemLocation cameraItemLocation = {
				.storeIndex = frameStoreItem.pStore->descriptorArrayElement,
				.itemIndex = frameStoreItem.index
			};

			global.renderStyle = renderStyle;

			global.cameraItemIndex = cameraItemLocation.itemIndex;
			global.cameraStoreIndex = cameraItemLocation.storeIndex;

			auto pointLightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::PointLight>>();
			auto directionalLightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::DirectionalLight>>();
			auto spotlightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::Spotlight>>();

			for (const auto& entity: pointLightStoreItems) {
				auto pointLightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::PointLight>>(entity);
				global.pointLightArrayIndex = pointLightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			global.pointLightCount = pointLightSystem.getItemCount();

			for (const auto& entity: directionalLightStoreItems) {
				auto directionalLightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::DirectionalLight>>(
					entity
				);
				global.directionalLightArrayIndex = directionalLightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			global.directionalLightCount = directionalLightSystem.getItemCount();

			for (const auto& entity: spotlightStoreItems) {
				auto spotlightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::Spotlight>>(entity);
				global.spotlightArrayIndex = spotlightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			global.spotlightCount = spotlightSystem.getItemCount();
			*frameState.Global = global;

			GlobalSynchronizationState.Update(frameIndex);
		}
	}
}