#pragma once
#include <vulkan/vulkan.hpp>
#include "../../Engine/EngineState.hpp"
#include "../../Devices/Device.hpp"

namespace drk::Textures::Systems {
	class TextureSystem {
	protected:
		const Devices::DeviceContext& deviceContext;
		Engine::EngineState& EngineState;
		entt::registry& registry;

	public:
		TextureSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void UploadTextures();
		static entt::entity
		copyTextureEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);
	};
}