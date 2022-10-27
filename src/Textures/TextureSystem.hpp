#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Graphics/EngineState.hpp"
#include "../Devices/Device.hpp"

namespace drk::Textures {
	class TextureSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Graphics::EngineState& EngineState;
		entt::registry& Registry;

	public:
		TextureSystem(const Devices::DeviceContext& deviceContext, Graphics::EngineState& engineState, entt::registry& registry);
		void UploadTextures();
	};
}