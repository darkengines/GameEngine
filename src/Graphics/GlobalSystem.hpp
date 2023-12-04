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
		uint32_t pointLightCount;
		uint32_t directionalLightCount;
		uint32_t spotlightCount;
		SynchronizationState<Engine::Models::Global> GlobalSynchronizationState;
	public:
		GlobalSystem(Engine::EngineState& engineState, entt::registry& registry);

		void SetCamera(entt::entity cameraEntity);

		void Update();
		void setPointLightCount(uint32_t pointLightCount);
		uint32_t getPointLightCount();
		void setDirectionalLightCount(uint32_t directionalLightCount);
		uint32_t getDirectionalLightCount();
		void setSpotlightCount(uint32_t spotlightCount);
		uint32_t getSpotlightCount();
	};
}