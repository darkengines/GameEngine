#pragma once

#include "SynchronizationState.hpp"
#include "../Engine/EngineState.hpp"
#include <entt/entity/entity.hpp>
#include "../Cameras/Models/Camera.hpp"
#include "../Lights/Systems/DirectionalLightSystem.hpp"
#include "../Lights/Systems/PointLightSystem.hpp"
#include "../Lights/Systems/SpotlightSystem.hpp"
#include <boost/signals2/signal.hpp>

namespace drk::Graphics {
	class GlobalSystem {
	protected:
		Engine::EngineState& EngineState;
		entt::registry& Registry;
		entt::entity CameraEntity;
		Lights::Systems::DirectionalLightSystem& directionalLightSystem;
		Lights::Systems::SpotlightSystem& spotlightSystem;
		Lights::Systems::PointLightSystem& pointLightSystem;
		uint32_t pointLightBufferIndex;
		uint32_t pointLightCount;
		uint32_t directionalLightCount;
		uint32_t directionalLightBufferIndex;
		uint32_t spotlightCount;
		uint32_t spotlightBufferIndex;
		uint32_t renderStyle;
	public:
		boost::signals2::signal<void(entt::entity)> cameraChanged;
		SynchronizationState<Engine::Models::Global> GlobalSynchronizationState;
		GlobalSystem(
			Engine::EngineState& engineState,
			entt::registry& registry,
			Lights::Systems::DirectionalLightSystem& directionalLightSystem,
			Lights::Systems::SpotlightSystem& spotlightSystem,
			Lights::Systems::PointLightSystem& pointLightSystem
		);

		void setCamera(entt::entity cameraEntity);

		void update();
		void setRenderStyle(uint32_t style);
	};
}