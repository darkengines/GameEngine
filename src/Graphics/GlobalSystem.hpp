#pragma once

#include "SynchronizationState.hpp"
#include "EngineState.hpp"
#include <entt/entity/entity.hpp>
#include "../Cameras/Models/Camera.hpp"

namespace drk::Graphics {
	class GlobalSystem {
	protected:
		EngineState *EngineState;
		entt::entity CameraEntity;
		SynchronizationState<Models::Global> GlobalSynchronizationState;
	public:
		GlobalSystem(drk::Graphics::EngineState *engineState);

		void SetCamera(entt::entity cameraEntity);

		void Update();
	};
}