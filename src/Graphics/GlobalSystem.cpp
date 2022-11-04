#include "GlobalSystem.hpp"

namespace drk::Graphics {

	GlobalSystem::GlobalSystem(Engine::EngineState& engineState, entt::registry& registry)
		: EngineState(engineState), Registry(registry), GlobalSynchronizationState(EngineState.getFrameCount()) {

	}

	void GlobalSystem::SetCamera(entt::entity cameraEntity) {
		CameraEntity = cameraEntity;
		EngineState.CameraEntity = CameraEntity;
		GlobalSynchronizationState.Reset();
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
			frameState.Global->cameraItemLocation = cameraItemLocation;
			GlobalSynchronizationState.Update(frameIndex);
		}
	}
}