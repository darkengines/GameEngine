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

	void GlobalSystem::setPointLightBufferIndex(uint32_t pointLightBufferIndex) {
		this->pointLightBufferIndex = pointLightBufferIndex;
		GlobalSynchronizationState.Reset();
	}
	uint32_t GlobalSystem::getPointLightBufferIndex() {
		return pointLightBufferIndex;
	}
	void GlobalSystem::setDirectionalLightBufferIndex(uint32_t directionalLightBufferIndex) {
		this->directionalLightBufferIndex = directionalLightBufferIndex;
		GlobalSynchronizationState.Reset();
	}
	uint32_t GlobalSystem::getDirectionalLightBufferIndex() {
		return directionalLightBufferIndex;
	}
	void GlobalSystem::setSpotlightBufferIndex(uint32_t spotlightdBufferIndex) {
		this->spotlightBufferIndex = spotlightdBufferIndex;
		GlobalSynchronizationState.Reset();
	}
	uint32_t GlobalSystem::getSpotlightBufferIndex() {
		return spotlightBufferIndex;
	}

	void GlobalSystem::Update() {
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

			global.cameraItemIndex = cameraItemLocation.itemIndex;
			global.cameraStoreIndex = cameraItemLocation.storeIndex;

			auto pointLightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::PointLight>>();
			auto directionalLightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::DirectionalLight>>();
			auto spotlightStoreItems = Registry.view<Stores::StoreItem<Lights::Models::Spotlight>>();

			for (const auto& entity : pointLightStoreItems) {
				auto pointLightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::PointLight>>(entity);
				global.pointLightArrayIndex = pointLightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			global.pointLightCount = pointLightCount;

			for (const auto& entity : directionalLightStoreItems) {
				auto directionalLightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::DirectionalLight>>(entity);
				global.directionalLightArrayIndex = directionalLightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			global.directionalLightCount = directionalLightCount;

			for (const auto& entity : spotlightStoreItems) {
				auto spotlightStoreItem = Registry.get<Stores::StoreItem<Lights::Models::Spotlight>>(entity);
				global.spotlightArrayIndex = spotlightStoreItem.frameStoreItems[frameIndex].pStore->descriptorArrayElement;
				break;
			}
			global.spotlightCount = spotlightCount;

			*frameState.Global = global;

			GlobalSynchronizationState.Update(frameIndex);
		}
	}
}