#pragma once

#include "SynchronizationState.hpp"
#include "../Engine/EngineState.hpp"
#include <entt/entity/entity.hpp>
#include "../Cameras/Models/Camera.hpp"

namespace drk::Graphics {
	class GlobalSystem {
	protected:
		Engine::EngineState& EngineState;
		entt::registry& Registry;
		entt::entity CameraEntity;
		SynchronizationState<Engine::Models::Global> GlobalSynchronizationState;
	public:
		GlobalSystem(Engine::EngineState& engineState, entt::registry& registry);

		void SetCamera(entt::entity cameraEntity);

		void Update();
	};
}