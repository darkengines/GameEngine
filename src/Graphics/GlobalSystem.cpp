#include "GlobalSystem.hpp"

namespace drk::Graphics {

	GlobalSystem::GlobalSystem(Graphics::EngineState& engineState, entt::registry& registry)
		: EngineState(engineState), Registry(registry), GlobalSynchronizationState(EngineState.FrameStates.size()) {

	}

	void GlobalSystem::SetCamera(entt::entity cameraEntity) {
		CameraEntity = cameraEntity;
		EngineState.CameraEntity = CameraEntity;
		GlobalSynchronizationState.Reset();
	}

	void GlobalSystem::Update() {
		if (GlobalSynchronizationState.ShouldUpdate(EngineState.FrameIndex)) {
			const auto& camera = Registry.get<Stores::StoreItem<Cameras::Models::Camera>>(CameraEntity);

			Graphics::Models::StoreItemLocation cameraItemLocation = {
				.storeIndex = camera.frameStoreItems[EngineState.FrameIndex].pStore->descriptorArrayElement,
				.itemIndex = camera.frameStoreItems[EngineState.FrameIndex].index
			};
			EngineState.FrameStates[EngineState.FrameIndex].Global->cameraItemLocation = cameraItemLocation;
			GlobalSynchronizationState.Update(EngineState.FrameIndex);
		}
	}
}