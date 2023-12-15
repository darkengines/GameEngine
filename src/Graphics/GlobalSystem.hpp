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
		uint32_t pointLightBufferIndex;
		uint32_t pointLightCount;
		uint32_t directionalLightCount;
		uint32_t directionalLightBufferIndex;
		uint32_t spotlightCount;
		uint32_t spotlightBufferIndex;
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
		void setPointLightBufferIndex(uint32_t pointLightCount);
		uint32_t getPointLightBufferIndex();
		void setDirectionalLightBufferIndex(uint32_t directionalLightCount);
		uint32_t getDirectionalLightBufferIndex();
		void setSpotlightBufferIndex(uint32_t spotlightCount);
		uint32_t getSpotlightBufferIndex();
	};
}