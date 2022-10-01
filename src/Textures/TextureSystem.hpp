#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Graphics/EngineState.hpp"
#include "../Devices/Device.hpp"

namespace drk::Textures {
	class TextureSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;

	public:
		TextureSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		void UploadTextures();
	};
}