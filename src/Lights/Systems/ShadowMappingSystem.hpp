#pragma once

#include "../../Common/KGuillotineAllocator.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/Texture.hpp"
#include "../../Engine/EngineState.hpp"
#include <optional>

namespace drk::Lights::Systems {
	class ShadowMappingSystem {
	protected:
		Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
	public:
		std::optional<Devices::Texture> shadowMappingTexture;
		ShadowMappingSystem(Devices::DeviceContext& deviceContext, Engine::EngineState& engineState);
		guillotine::Allocator shadowMapAllocator;
		static Devices::Texture BuildShadowMappingTexture(Devices::DeviceContext& deviceContext, Engine::EngineState& engineState);
		static const uint32_t shadowMapWidth = 8192;
		static const uint32_t shadowMapHeight = 8192;
	};
}