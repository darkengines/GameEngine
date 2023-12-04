#include "GlobalSystem.hpp"
#include "../Lights/Models/PointLight.hpp"
#include "../Lights/Models/DirectionalLight.hpp"
#include "../Lights/Models/Spotlight.hpp"

namespace drk::Graphics {

	GlobalSystem::GlobalSystem(Engine::EngineState& engineState, entt::registry& registry)
		: EngineState(engineState), Registry(registry), GlobalSynchronizationState(EngineState.getFrameCount()) {

	}

	void GlobalSystem::SetCamera(entt::entity cameraEntity) {
		CameraEntity = cameraEntity;
		EngineState.CameraEntity = CameraEntity;
		GlobalSynchronizationState.Reset();
	}

	void GlobalSystem::setPointLightCount(uint32_t pointLightCount) {
		this->pointLightCount = pointLightCount;
		GlobalSynchronizationState.Reset();
	}

	uint32_t GlobalSystem::getPointLightCount() {
		return pointLightCount;
	}

	void GlobalSystem::setDirectionalLightCount(uint32_t directionalLightCount) {
		this->directionalLightCount = directionalLightCount;
		GlobalSynchronizationState.Reset();
	}

	uint32_t GlobalSystem::getDirectionalLightCount() {
		return directionalLightCount;
	}

	void GlobalSystem::setSpotlightCount(uint32_t spotlightCount) {
		this->spotlightCount = spotlightCount;
		GlobalSynchronizationState.Reset();
	}

	uint32_t GlobalSystem::getSpotlightCount() {
		return spotlightCount;
	}

	void GlobalSystem::Update() {
		if (GlobalSynchronizationState.ShouldUpdate(EngineState.getFrameIndex())) {
			const auto& camera = Registry.get<Stores::StoreItem<Cameras::Models::Camera>>(CameraEntity);

			const auto frameIndex = EngineState.getFrameIndex();
			const auto& frameStoreItem = camera.frameStoreItems[frameIndex];
			auto& frameState = EngineState.getCurrentFrameState();
			Stores::Models::StoreItemLocation cameraItemLocation = {
				.storeIndex = frameStoreItem.pStore->descriptorArrayElement,
				.itemIndex = frameStoreItem.index
			};

			auto pointLightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::PointLight>>();
			auto directionalLightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::DirectionalLight>>();
			auto spotlightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::Spotlight>>();

			for(const auto& entity: pointLightStoreItems) {
				auto pointLightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::PointLight>>(entity);
				frameState.Global->pointLightArrayIndex = pointLightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			frameState.Global->pointLightCount = pointLightCount;

			for(const auto& entity: directionalLightStoreItems) {
				auto directionalLightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::DirectionalLight>>(entity);
				frameState.Global->directionalLightArrayIndex = directionalLightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			frameState.Global->directionalLightCount = directionalLightCount;

			for(const auto& entity: spotlightStoreItems) {
				auto spotlightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::Spotlight>>(entity);
				frameState.Global->spotlightArrayIndex = spotlightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			frameState.Global->spotlightCount = spotlightCount;

			frameState.Global->cameraItemLocation = cameraItemLocation;
			GlobalSynchronizationState.Update(frameIndex);
		}
	}
}